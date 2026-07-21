/*
 * Hidden line elimination unit based on
 * Ian O. Angell's algorithm.
 * Translated from Fortran IV and modified
 * to suit modern C with the help of ChatGPT.
 *
*/

#include "../Inc/lib3d_hle.h"
#include "../Inc/lib3d_util.h"
#include "../Inc/lib3d_transform.h"	// for transformVertexIntoViewSpace()

// 
// Reset the interval list.
// Initially entire edge is visible.
// Equivalent to rmin=0.0, rmax=1.0.
// 
void l3d_interval_reset(
	l3d_interval_list_t *list)
{
	list->count = 1;

	list->interval[0].begin = l3d_floatToRational(0.0f);
	list->interval[0].end   = l3d_floatToRational(1.0f);
}

// 
// Subtract given interval [hidden_begin, hidden_end]
// from given list of intervals.
// 
// Return L3D_BUFF_OVF if maximum number of intervals
// has been reached, L3D_OK otherwise.
// 
// This function was mostly written by ChatGPT.
// 
l3d_err_t l3d_interval_subtract(
	l3d_interval_list_t *list,
	l3d_rtnl_t hidden_begin,
	l3d_rtnl_t hidden_end)
{
	uint8_t i = 0;

	while (i < list->count)
	{
		l3d_interval_t current = list->interval[i];

		// Case 1: no overlap -> nothing changes
		// 
		// visible:
		// |------|
		// hidden:
		//              |------|
		// 
		if (current.end <= hidden_begin ||
			current.begin >= hidden_end)
		{
			i++;
			continue;
		}

		// Case 2: entire interval disappears
		// visible:
		// |---------|
		// hidden:
		// |--------------|
		if (hidden_begin <= current.begin &&
			hidden_end >= current.end)
		{
			for (uint8_t j=i+1; j<list->count; j++)
			{
				list->interval[j-1] = list->interval[j];
			}

			list->count--;

			continue;
		}

		// Case 3: hidden interval splits visible interval
		//         (the only place where interval array grows)
		// before:
		// |--------------------|
		// after:
		// |-----|
		//         |-----------|
		if (hidden_begin > current.begin &&
			hidden_end < current.end)
		{
			if (list->count ==
				L3D_HLE_MAX_INTERVALS)
			{
				return L3D_BUFF_OVF;
			}

			for (uint8_t j = list->count; j > i+1; j--)
			{
				list->interval[j] = list->interval[j-1];
			}

			list->interval[i].end = hidden_begin;

			list->interval[i+1].begin = hidden_end;

			list->interval[i+1].end = current.end;

			list->count++;

			return L3D_OK;
		}

		// Case 4: trim left
		// before
		// |-------------|
		// hidden
		// |------|
		if (hidden_begin <= current.begin)
		{
			list->interval[i].begin =
				hidden_end;

			i++;

			continue;
		}

		// Case 5: trim right
		// before
		// |-------------|
		// hidden
		//           |---------|
		list->interval[i].end = hidden_begin;

		i++;
	}

	return L3D_OK;
}

// 
// Check if bounding boxes of given edge and
// given triangle are overlapping.
// 
bool l3d_hle_bboxOverlap(
    const l3d_vec4_t *e_v0,
    const l3d_vec4_t *e_v1,
	const l3d_vec4_t *tri_v0,
	const l3d_vec4_t *tri_v1,
	const l3d_vec4_t *tri_v2)
{
	// Calculate the bounding box of given edge
    l3d_rtnl_t edge_xmin = (e_v0->x < e_v1->x) ? e_v0->x : e_v1->x;
    l3d_rtnl_t edge_xmax = (e_v0->x > e_v1->x) ? e_v0->x : e_v1->x;

    l3d_rtnl_t edge_ymin = (e_v0->y < e_v1->y) ? e_v0->y : e_v1->y;
    l3d_rtnl_t edge_ymax = (e_v0->y > e_v1->y) ? e_v0->y : e_v1->y;

	// Calculate the bounding box of given triangle
    l3d_rtnl_t face_xmin = tri_v0->x;
    l3d_rtnl_t face_xmax = tri_v0->x;

    l3d_rtnl_t face_ymin = tri_v0->y;
    l3d_rtnl_t face_ymax = tri_v0->y;

	if (tri_v1->x < face_xmin)
		face_xmin = tri_v1->x;
	if (tri_v1->x > face_xmax)
		face_xmax = tri_v1->x;

	if (tri_v2->x < face_xmin)
		face_xmin = tri_v2->x;
	if (tri_v2->x > face_xmax)
		face_xmax = tri_v2->x;
	
	if (tri_v1->y < face_ymin)
		face_ymin = tri_v1->y;
	if (tri_v1->y > face_ymax)
		face_ymax = tri_v1->y;

	if (tri_v2->y < face_ymin)
		face_ymin = tri_v2->y;
	if (tri_v2->y > face_ymax)
		face_ymax = tri_v2->y;

    if (edge_xmax < face_xmin) return false;
    if (edge_xmin > face_xmax) return false;

    if (edge_ymax < face_ymin) return false;
    if (edge_ymin > face_ymax) return false;

    return true;
}

// 
// Return s if edges P=(p0, p1) and
// Q=(q0, q1) intersect, where s
// (in [0.0, 1.0]) is the scale/length
// of the vector P(s), where P intersects Q.
// 
// P(s)=P0+s(P1-P0)
// Q(t)=Q0+t(Q1-Q0)
// where P is the tested edge
// and Q is one of the triangle edges
// that P is tested against.
// 
// This corresponds to labels 7-10
// in Angell's algorithm.
// 
//                    q0
//                    /
//            <--s-->/
//         p0 ------x------- p1
//                 /
//                /
//               q1
// 
bool l3d_segmentLineIntersection(
    const l3d_vec4_t *p0,
    const l3d_vec4_t *p1,
    const l3d_vec4_t *q0,
    const l3d_vec4_t *q1,
    l3d_rtnl_t *s)
{
	// Compute vector P = P1 - P0
    l3d_rtnl_t px = p1->x - p0->x;
    l3d_rtnl_t py = p1->y - p0->y;

	// Compute vector Q = Q1 - Q0
    l3d_rtnl_t qx = q1->x - q0->x;
    l3d_rtnl_t qy = q1->y - q0->y;

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
	l3d_rtnl_t det = l3d_fixedMul(px, qy) - l3d_fixedMul(py, qx);
	// L3D_DEBUG_PRINT("pxqy=%f\n", l3d_rationalToFloat(l3d_fixedMul(px, qy)));
	// L3D_DEBUG_PRINT("pyqx=%f\n", l3d_rationalToFloat(l3d_fixedMul(py, qx)));
	// L3D_DEBUG_PRINT("pxqy-pyqx=%f\n",
	// 	l3d_rationalToFloat(l3d_fixedMul(px, qy) - l3d_fixedMul(py, qx)));
#else
	l3d_rtnl_t det = px*qy - py*qx;
#endif

    if(l3d_abs(det) < L3D_EPSILON_RTNL)
	{
		// L3D_DEBUG_PRINT("det = 0 => Lines parallel.\n");
		return false;
	}

    l3d_rtnl_t rx = q0->x - p0->x;
    l3d_rtnl_t ry = q0->y - p0->y;

	// L3D_DEBUG_PRINT("P=(%f,%f)\n", l3d_rationalToFloat(px), l3d_rationalToFloat(py));
	// L3D_DEBUG_PRINT("Q=(%f,%f)\n", l3d_rationalToFloat(qx), l3d_rationalToFloat(qy));
	// L3D_DEBUG_PRINT("R=(%f,%f)\n", l3d_rationalToFloat(rx), l3d_rationalToFloat(ry));
	// L3D_DEBUG_PRINT("det=%f\n", l3d_rationalToFloat(det));

	l3d_rtnl_t t;

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
	l3d_rtnl_t tmp;

	// tmp = l3d_fixedMul(qx, ry) - l3d_fixedMul(qy, rx);
	// *s = l3d_fixedDiv(tmp, det);

	// tmp = l3d_fixedMul(px, ry) - l3d_fixedMul(py, rx);
    // t = l3d_fixedDiv(tmp, det);

	tmp = l3d_fixedMul(rx, qy) - l3d_fixedMul(ry, qx);
	// L3D_DEBUG_PRINT("cross(R,Q)=%f\n", l3d_rationalToFloat(tmp));

	*s = l3d_fixedDiv(tmp, det);

	tmp = l3d_fixedMul(rx, py) - l3d_fixedMul(ry, px);
	// L3D_DEBUG_PRINT("cross(R,P)=%f\n", l3d_rationalToFloat(tmp));

	t = l3d_fixedDiv(tmp, det);
#else
    // *s = (qx*ry - qy*rx) / det;
    // t = (px*ry - py*rx) / det;

	*s = (rx*qy - ry*qx) / det;
	t  = (rx*py - ry*px) / det;
#endif

	bool result =  (*s >= l3d_floatToRational(0.0f) &&
					*s <= l3d_floatToRational(1.0f) &&
					t >= l3d_floatToRational(0.0f) &&
					t <= l3d_floatToRational(1.0f));

	// L3D_DEBUG_PRINT("s = %.3f, t = %.3f, intersects edge?: %d\n",
	// 	l3d_rationalToFloat(*s), l3d_rationalToFloat(t),
	// 	result);

	// t not in <0, 1> => triangle edge intersects tested line,
	// 					  but not the fragment being the tested edge

	return result;
}

// 
// Compute the intersection interval [rmin, rmax]
// of given edge with given triangle.
// rmin and rmax are given as a fraction of distance
// along the edge starting from its first vertex (0.0f)
// to the second (1.0f). 
// All vertices are in screen space (projected, 2D).
// 
bool l3d_hle_findOverlap(
    const l3d_vec4_t *edge0,
    const l3d_vec4_t *edge1,
    const l3d_vec4_t *tri0,
    const l3d_vec4_t *tri1,
    const l3d_vec4_t *tri2,
    l3d_rtnl_t *rmin,
    l3d_rtnl_t *rmax)
{
    *rmin = L3D_RTNL_ONE;
    *rmax = L3D_RTNL_ZERO;

    l3d_rtnl_t s;

	// Test if the edge intersects every triangle edge
	// and compute the maximum interval.

    if (l3d_segmentLineIntersection(
            edge0, edge1,
            tri0, tri1,
            &s))
    {
        if (s < *rmin) *rmin = s;
        if (s > *rmax) *rmax = s;
    }

    if (l3d_segmentLineIntersection(
            edge0, edge1,
            tri1, tri2,
            &s))
    {
        if (s < *rmin) *rmin = s;
        if (s > *rmax) *rmax = s;
    }

    if (l3d_segmentLineIntersection(
            edge0, edge1,
            tri2, tri0,
            &s))
    {
        if (s < *rmin) *rmin = s;
        if (s > *rmax) *rmax = s;
    }

	// Account for cases where any of edge vertices
	// lies inside the triangle.

	bool is_e0_in_face = l3d_isPointInTri(edge0, tri0, tri1, tri2);
	bool is_e1_in_face = l3d_isPointInTri(edge1, tri0, tri1, tri2);
	
	if (is_e0_in_face)
		*rmin = L3D_RTNL_ZERO;
	
	if (is_e1_in_face)
		*rmax = L3D_RTNL_ONE;

	// If lines intersect at interval outside of [0.0f, 1.0f],
	// then the intersection point lies outside of any of the edges,
	// so they do not intersect.

    if (*rmax <= L3D_RTNL_ZERO)
        return false;

    if (*rmin >= L3D_RTNL_ONE)
        return false;
	
	// Clamp rmin and rmax 

    if (*rmin < L3D_RTNL_ZERO)
        *rmin = L3D_RTNL_ZERO;

    if (*rmax > L3D_RTNL_ONE)
        *rmax = L3D_RTNL_ONE;

	// The result [rmin, rmax] is valid if numbers make sense.
    return (*rmax > *rmin);
}

// 
// Helper function for l3d_isPointInTri().
// 
// Source - https://stackoverflow.com/a/2049593
// Posted by Kornel Kisielewicz, modified by community.
// See post 'Timeline' for change history
// Retrieved 2026-07-20, License - CC BY-SA 4.0
l3d_rtnl_t l3d_isPointInTriSignHelper(
	const l3d_vec4_t *v0,
	const l3d_vec4_t *v1,
	const l3d_vec4_t *v2)
{
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
    return l3d_fixedMul((v0->x - v2->x)m (v1->y - v2->y)) - l3d_fixedMul((v1->x - v2->x) * (v0->y - v2->y));
#else
	return (v0->x - v2->x) * (v1->y - v2->y) - (v1->x - v2->x) * (v0->y - v2->y);
#endif /* L3D_USE_FIXED_POINT_ARITHMETIC */
}

// 
// Test if given point lies inside
// given triangle in screen space.
// 
// v						- point to be tested
// tri_v0, tri_v1, tri_v2	- triangle vertices
// 
// Original source - https://stackoverflow.com/a/2049593
// Posted by Kornel Kisielewicz, modified by community.
// See post 'Timeline' for change history
// Retrieved 2026-07-20, License - CC BY-SA 4.0
// 
bool l3d_isPointInTri(
		const l3d_vec4_t *v,
		const l3d_vec4_t *tri_v0,
		const l3d_vec4_t *tri_v1,
		const l3d_vec4_t *tri_v2)
{
	l3d_rtnl_t d1, d2, d3;
    bool has_neg, has_pos;

    d1 = l3d_isPointInTriSignHelper(v, tri_v0, tri_v1);
    d2 = l3d_isPointInTriSignHelper(v, tri_v1, tri_v2);
    d3 = l3d_isPointInTriSignHelper(v, tri_v2, tri_v0);

    has_neg = (d1 < L3D_RTNL_ZERO) || (d2 < L3D_RTNL_ZERO) || (d3 < L3D_RTNL_ZERO);
    has_pos = (d1 > L3D_RTNL_ZERO) || (d2 > L3D_RTNL_ZERO) || (d3 > L3D_RTNL_ZERO);

    return !(has_neg && has_pos);
}

// 
// Linear interpolation between two variables a and b given a fraction f.
// 
// Original source - https://stackoverflow.com/q/4353525
// Posted by Thomas O, modified by community. See post 'Timeline' for change history
// Retrieved 2026-07-14, License - CC BY-SA 3.0
// 
l3d_rtnl_t l3d_lerp(l3d_rtnl_t a, l3d_rtnl_t b, l3d_rtnl_t f)
{
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
	return l3d_fixedMul(a, (L3D_RTNL_ONE - f)) + l3d_fixedMul(b, f);
#else
	return (a * (1.0 - f)) + (b * f);
#endif /* L3D_USE_FIXED_POINT_ARITHMETIC */
}

// 
// Linear interpolation between two vectors a and b, given a fraction f.
// 
l3d_vec4_t l3d_vecLerp(const l3d_vec4_t *a, const l3d_vec4_t *b, l3d_rtnl_t f)
{
	l3d_vec4_t result, tmp;

	result = l3d_vec4_mul(a, (L3D_RTNL_ONE - f));
	tmp = l3d_vec4_mul(b, f);
	result = l3d_vec4_add(&result, &tmp);

	return result;
}

// 
// Draw only visible intervals of given edge.
// v0, v1	- edge vertices
// list		- list of visible intervals
// 
// This procedure corresponds to
// labels 18-19 in Angell's algorithm.
// 
void l3d_drawVisibleIntervals(
	const l3d_vec4_t *v0,
	const l3d_vec4_t *v1,
	l3d_interval_list_t *list,
	l3d_colour_t colour)
{
	for(uint16_t i = 0; i < list->count; i++)
	{
		l3d_rtnl_t t0 = list->interval[i].begin;
		l3d_rtnl_t t1 = list->interval[i].end;

		l3d_vec4_t p0 = l3d_vecLerp(v0, v1, t0);
		l3d_vec4_t p1 = l3d_vecLerp(v0, v1, t1);

		l3d_drawLineCallback(
			l3d_rationalToInt32(p0.x), l3d_rationalToInt32(p0.y),
			l3d_rationalToInt32(p1.x), l3d_rationalToInt32(p1.y),
			colour);
	}
}

// 
// Test if given edge belongs to given face.
// Testing vertex indices might be sufficient.
// 
bool l3d_hle_edgeBelongsToFace(
	uint16_t edge_v0_idx, uint16_t edge_v1_idx,
	uint16_t tri_v0_idx, uint16_t tri_v1_idx, uint16_t tri_v2_idx)
{
	if ((edge_v0_idx == tri_v0_idx && edge_v1_idx == tri_v1_idx) ||
		(edge_v1_idx == tri_v0_idx && edge_v0_idx == tri_v1_idx) ||
		(edge_v0_idx == tri_v1_idx && edge_v1_idx == tri_v2_idx) ||
		(edge_v1_idx == tri_v1_idx && edge_v0_idx == tri_v2_idx) ||
		(edge_v0_idx == tri_v2_idx && edge_v1_idx == tri_v0_idx) ||
		(edge_v1_idx == tri_v2_idx && edge_v0_idx == tri_v0_idx))
	{
		return true;
	}

	return false;
}

// 
// Given parameter screen_r in screen space,
// compute parameter world_r in world space
// using the Newton step formula:
// t_{n+1} = t_n - \frac{f(t_n)}{f'(t_n)}
// where error = f(t_n),
// derivative = f'(t_n), and
// f(t) = x_{screen} - x_{target} or
// f(t) = x_{screen} - x_{target},
// whichever is numerically better.
// This procedure solves for f(t) = 0.
// 
// The parameter (in [0,1]) corresponds to the relative
// distance from first edge vertex to the second one.
// 
// world0, world1	- edge vertices in world space
// proj0, proj1		- edge vertices in screen space
// mat_view			- view matrix
// mat_proj			- projection matrix
// screen_r			- parameter along the edge
// 					  in screen space
// world_r			- parameter along the edge
// 					  in world space
// 
void l3d_projectEdgeParameter(
    const l3d_vec4_t *world0,
    const l3d_vec4_t *world1,
    const l3d_vec4_t *proj0,
    const l3d_vec4_t *proj1,
	const l3d_mat4x4_t *mat_view,
	const l3d_mat4x4_t *mat_proj,
    l3d_rtnl_t screen_r,
    l3d_rtnl_t *world_r)
{
    // Desired screen point M on the projected edge
    l3d_vec4_t screen_mid =
        l3d_vecLerp(proj0, proj1, screen_r);

    // Initial guess: use the same parameter
    l3d_rtnl_t t = screen_r;

    // Perspective correction using Newton iterations
    // According to ChatGPT, 3 iterations are usually enough
    for(uint8_t iter = 0; iter < 3; iter++)
    {
        // 3D point on edge
        l3d_vec4_t p = l3d_vecLerp(world0, world1, t);

		// Compute the error:

		// Project current 3D point to screen space
		l3d_vec4_t screen_p =
			transformVertexIntoViewSpace(&p, mat_view, mat_proj);

		// Choose the dominant axis
		// (for better numerical computations)
		l3d_rtnl_t dx =
			l3d_abs(proj1->x - proj0->x);

		l3d_rtnl_t dy =
			l3d_abs(proj1->y - proj0->y);

		bool use_x = (dx > dy);

		// Scalar error
		l3d_rtnl_t error =
			use_x
			? (screen_p.x - screen_mid.x)
			: (screen_p.y - screen_mid.y);

		// Compute the numerical derivative:
        l3d_rtnl_t dt = l3d_floatToRational(0.001f);

		l3d_vec4_t p2 =
			l3d_vecLerp(world0, world1, t + dt);

		l3d_vec4_t screen_p2 =
			transformVertexIntoViewSpace(&p2, mat_view, mat_proj);

#if L3D_USE_FIXED_POINT_ARITHMETIC
		l3d_rtnl_t deriv =
			use_x
			? l3d_fixedDiv((screen_p2.x - screen_p.x), dt)
			: l3d_fixedDiv((screen_p2.y - screen_p.y), dt);
#else
		l3d_rtnl_t deriv =
			use_x
			? (screen_p2.x - screen_p.x) / dt
			: (screen_p2.y - screen_p.y) / dt;
#endif /* L3D_USE_FIXED_POINT_ARITHMETIC */

        // Newton step
#if L3D_USE_FIXED_POINT_ARITHMETIC
		t = t - l3d_fixedDiv(error, deriv);
#else
        t = t - error / deriv;
#endif

        // Clamp the parameter to [0, 1]
        if(t < l3d_floatToRational(0.0f))
			t = l3d_floatToRational(0.0f);
        if(t > l3d_floatToRational(1.0f))
            t = l3d_floatToRational(1.0f);
    }

    *world_r = t;
}

// 
// Perform scene rendering in wireframe mode
// with hidden line elimination.
// 
l3d_err_t l3d_render_hle(const l3d_scene_t *scene)
{
	l3d_interval_list_t il;
	l3d_camera_t *cam_p = l3d_scene_getActiveCamera(scene);

	// For each edge
	for (uint16_t edge_data_idx = 0; edge_data_idx < scene->model_edge_count * 3; edge_data_idx += 3)
	{
		// L3D_DEBUG_PRINT("Edge %d:\n", edge_data_idx / 3);

		const uint16_t e_v0_idx = scene->model_edge_data[edge_data_idx + 0];
		const uint16_t e_v1_idx = scene->model_edge_data[edge_data_idx + 1];

		// L3D_DEBUG_PRINT("Vertex IDs: (%d, %d):\n", e_v0_idx, e_v1_idx);

		const l3d_vec4_t *e_v0_world_p = &(scene->vertices_world[e_v0_idx]);
		const l3d_vec4_t *e_v1_world_p = &(scene->vertices_world[e_v1_idx]);

		const l3d_vec4_t *e_v0_proj_p = &(scene->vertices_projected[e_v0_idx]);
		const l3d_vec4_t *e_v1_proj_p = &(scene->vertices_projected[e_v1_idx]);

		l3d_interval_reset(&il);

		// For each face
		for (uint16_t tri_idx = 0; tri_idx < scene->model_tri_count * 3; tri_idx += 3)
		{
			// L3D_DEBUG_PRINT("Tri %d:\n", tri_idx / 3);

			const uint16_t tri_v0_idx = scene->model_tri_data[tri_idx + 0];
			const uint16_t tri_v1_idx = scene->model_tri_data[tri_idx + 1];
			const uint16_t tri_v2_idx = scene->model_tri_data[tri_idx + 2];

			// L3D_DEBUG_PRINT("Vertex IDs: (%d, %d, %d):\n", tri_v0_idx, tri_v1_idx, tri_v2_idx);

			// Used only to compute plane equation
			const l3d_vec4_t *tri_v0_world_p = &(scene->vertices_world[tri_v0_idx]);
			const l3d_vec4_t *tri_v1_world_p = &(scene->vertices_world[tri_v1_idx]);
			const l3d_vec4_t *tri_v2_world_p = &(scene->vertices_world[tri_v2_idx]);

			// Used for rejection tests
			const l3d_vec4_t *tri_v0_proj_p = &(scene->vertices_projected[tri_v0_idx]);
			const l3d_vec4_t *tri_v1_proj_p = &(scene->vertices_projected[tri_v1_idx]);
			const l3d_vec4_t *tri_v2_proj_p = &(scene->vertices_projected[tri_v2_idx]);

			if (l3d_hle_edgeBelongsToFace(e_v0_idx, e_v1_idx,
										  tri_v0_idx, tri_v1_idx, tri_v2_idx))
			{
				// L3D_DEBUG_PRINT("Edge (%d, %d) belongs to tri (%d, %d, %d). Continuing.\n",
				// 				e_v0_idx, e_v1_idx, tri_v0_idx, tri_v1_idx, tri_v2_idx);
				continue;
			}

			// Perform quick rejection (cases A, B, and C in Angell's algorithm).
			// A simple boundingbox overlap check should be sufficient.
			if (!l3d_hle_bboxOverlap(e_v0_proj_p, e_v1_proj_p,
									 tri_v0_proj_p, tri_v1_proj_p, tri_v2_proj_p))
			{
				// L3D_DEBUG_PRINT("No bbox overlap for edge (%d, %d) and tri (%d, %d, %d). Continuing.\n",
				// 				e_v0_idx, e_v1_idx, tri_v0_idx, tri_v1_idx, tri_v2_idx);
				continue;
			}

			l3d_rtnl_t rmin, rmax;
			if (!l3d_hle_findOverlap(e_v0_proj_p, e_v1_proj_p,
									 tri_v0_proj_p, tri_v1_proj_p, tri_v2_proj_p,
									 &rmin, &rmax))
			{
				// L3D_DEBUG_PRINT("No overlap found for edge (%d, %d) and tri (%d, %d, %d). Continuing.\n",
				// 				e_v0_idx, e_v1_idx, tri_v0_idx, tri_v1_idx, tri_v2_idx);
				continue;
			}

			// Check if the face is closer to the camera than the edge.
			// If not, subtract hidden interval

			// Compute the point in the middle
			// of the interval [rmin, rmax] of the projected edge.
#if L3D_USE_FIXED_POINT_ARITHMETIC
			l3d_rtnl_t rmid = l3d_fixedDiv((rmin + rmax), 2);
#else
			l3d_rtnl_t rmid = (rmin + rmax) / 2;
#endif /*  */

			// Find a vertex in 3D corresponding to
			// the midpoint in screen space.
			// This point corresponds to (xhat, yhat, zhat)
			// in Angell's algorithm (labels 10-12),
			// but computed without the PHI parameter.
			l3d_rtnl_t world_r;
			l3d_projectEdgeParameter(
				e_v0_world_p, e_v1_world_p,
				e_v0_proj_p, e_v1_proj_p,
				&(scene->mat_view),
				&(scene->mat_proj),
				rmid,
				&world_r);
			
			l3d_vec4_t mid_world = l3d_vecLerp(e_v0_world_p,
											   e_v1_world_p,
											   world_r);
			
			// L3D_DEBUG_PRINT("rmin = %.3f, rmax = %.3f, rmid = %.3f, world_r = %.3f\n",
			// 	l3d_rationalToFloat(rmin), l3d_rationalToFloat(rmax), l3d_rationalToFloat(rmid), l3d_rationalToFloat(world_r));

			// Compute plane equation from currently tested face
			// and test if the camera and mid_world lie on the same
			// side of the plane.

			// TODO: move this to some cache storage not to repeat computation for every edge
			l3d_plane_t plane;
			l3d_plane_compute(&plane,
							  tri_v0_world_p, tri_v1_world_p, tri_v2_world_p);

			l3d_rtnl_t dist_edge = l3d_plane_eval(&plane, &mid_world);
			l3d_rtnl_t dist_cam = l3d_plane_eval(&plane, &(cam_p->local_pos));

			// If signs are equal, both lie on the same side of the plane,
			// so the face can not cover the edge and thus it is visible.
			if(l3d_sign(dist_edge) == l3d_sign(dist_cam))
			{
				// The face is behind the edge.
				// L3D_DEBUG_PRINT("Edge (%d, %d) in front of tri (%d, %d, %d). Continuing.\n",
				// 				e_v0_idx, e_v1_idx, tri_v0_idx, tri_v1_idx, tri_v2_idx);
				continue;
			}

			// L3D_DEBUG_PRINT("Edge (%d, %d) behind tri (%d, %d, %d) at (%.3f, %.3f). Subtracting interval.\n",
			// 				e_v0_idx, e_v1_idx, tri_v0_idx, tri_v1_idx, tri_v2_idx,
			// 				l3d_rationalToFloat(rmin), l3d_rationalToFloat(rmax));

			// Tested edge is covered by current face.
			// Subtract interval computed above in l3d_hle_findOverlap()
			l3d_interval_subtract(&il, rmin, rmax);
		}

		l3d_drawVisibleIntervals(e_v0_proj_p, e_v1_proj_p,
								 &il,
								 (l3d_colour_t)L3D_COLOUR_WHITE);
	}

	return L3D_OK;
}
