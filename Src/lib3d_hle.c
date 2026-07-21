/*
 * Hidden line elimination unit based on Ian O. Angell's
 * algorithm.
 * Translated from Fortran IV and modified to suit modern C
 * with the help of ChatGPT.
 *
*/

#include "../Inc/lib3d_hle.h"
#include "../Inc/lib3d_util.h"
#include "../Inc/lib3d_core.h"	// for transformVertexIntoViewSpace()

// 
// Initially entire edge is visible.
// Equivalent to: RMIN=0, RMAX=1
// 
void l3d_interval_reset(
	l3d_interval_list_t *list)
{
	list->count = 1;

	list->interval[0].begin = l3d_floatToRational(0.0f);
	list->interval[0].end   = l3d_floatToRational(1.0f);
}

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
	// Calculate bounding box of given edge
    l3d_rtnl_t edge_xmin = (e_v0->x < e_v1->x) ? e_v0->x : e_v1->x; //L3D_MIN(e_v0->x, e_v1->x);
    l3d_rtnl_t edge_xmax = (e_v0->x > e_v1->x) ? e_v0->x : e_v1->x; //L3D_MAX(e_v0->x, e_v1->x);

    l3d_rtnl_t edge_ymin = (e_v0->y < e_v1->y) ? e_v0->y : e_v1->y; //L3D_MIN(e_v0->y, e_v1->y);
    l3d_rtnl_t edge_ymax = (e_v0->y > e_v1->y) ? e_v0->y : e_v1->y; //L3D_MAX(e_v0->y, e_v1->y);

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
	
    // for(uint8_t i=1;i<vertex_count;i++)
    // {
    //     if(tri[i].x < face_xmin) face_xmin = tri[i].x;
    //     if(tri[i].x > face_xmax) face_xmax = tri[i].x;

    //     if(tri[i].y < face_ymin) face_ymin = tri[i].y;
    //     if(tri[i].y > face_ymax) face_ymax = tri[i].y;
    // }

    if (edge_xmax < face_xmin) return false;
    if (edge_xmin > face_xmax) return false;

    if (edge_ymax < face_ymin) return false;
    if (edge_ymin > face_ymax) return false;

    return true;
}

// 
// P(s)=P0+s(P1-P0)
// Q(t)=Q0+t(Q1-Q0)
// where P is the tested edge
// and Q is one triangle edge
// 
// Return s if edges intersect,
// where s in <0.0, 1.0> is the scale/length
// of the vector P(s) where the tested edge intersects
// the triangle edge given by verctor Q(t).
// 
// labels 7-10
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
// Compute intersection points (rmin, rmax)
// of given line with given triangle.
// rmin and rmax are as a fraction of given line.
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

	bool is_e0_in_face = l3d_isPointInTri(edge0, tri0, tri1, tri2);
	bool is_e1_in_face = l3d_isPointInTri(edge1, tri0, tri1, tri2);
	
	if (is_e0_in_face)
		*rmin = L3D_RTNL_ZERO;
	
	if (is_e1_in_face)
		*rmax = L3D_RTNL_ONE;

    if (*rmax <= L3D_RTNL_ZERO)
        return false;

    if (*rmin >= L3D_RTNL_ONE)
        return false;

    if (*rmin < L3D_RTNL_ZERO)
        *rmin = L3D_RTNL_ZERO;

    if (*rmax > L3D_RTNL_ONE)
        *rmax = L3D_RTNL_ONE;

    return (*rmax > *rmin);
	// return (l3d_abs(*rmax - *rmin) > L3D_EPSILON_RTNL);
	// return (l3d_abs(*rmax - *rmin) > l3d_floatToRational(0.01f));
}

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

// Source - https://stackoverflow.com/a/2049593
// Posted by Kornel Kisielewicz, modified by community.
// See post 'Timeline' for change history
// Retrieved 2026-07-20, License - CC BY-SA 4.0
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

// Source - https://stackoverflow.com/q/4353525
// Posted by Thomas O, modified by community. See post 'Timeline' for change history
// Retrieved 2026-07-14, License - CC BY-SA 3.0
// 
// Linear interpolation between two variables a and b given a fraction f.
// 
// float lerp(float a, float b, float f) 
// {
//     return (a * (1.0 - f)) + (b * f);
// }
l3d_rtnl_t l3d_lerp(l3d_rtnl_t a, l3d_rtnl_t b, l3d_rtnl_t f)
{
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
	return l3d_fixedMul(a, (L3D_RTNL_ONE - f)) + l3d_fixedMul(b, f);
#else
	return (a * (1.0 - f)) + (b * f);
#endif /* L3D_USE_FIXED_POINT_ARITHMETIC */
}

// 
// Linear interpolation between two vectors a and b given a fraction f.
// 
l3d_vec4_t l3d_vecLerp(const l3d_vec4_t *a, const l3d_vec4_t *b, l3d_rtnl_t f)
{
	l3d_vec4_t result, tmp;

	// return (a * (1.0 - f)) + (b * f);
	result = l3d_vec4_mul(a, (L3D_RTNL_ONE - f));
	tmp = l3d_vec4_mul(b, f);
	result = l3d_vec4_add(&result, &tmp);

	return result;
}

// 
// labels 10-12
// 
// l3d_err_t l3d_hle_computeMidpoint(
//     const l3d_vec4_t *v0_world,
//     const l3d_vec4_t *v1_world,
//     const l3d_vec4_t *v0_proj,
//     const l3d_vec4_t *v1_proj,
//     l3d_rtnl_t rmin,
//     l3d_rtnl_t rmax,
//     const l3d_camera_t *cam,
//     // l3d_hle_midpoint_t *mid
// 	l3d_vec4_t *mid_world,
// 	l3d_vec4_t *mid_screen)
// {
// 	l3d_rtnl_t rmid = (rmin + rmax) / 2;

// 	// *mid_world = l3d_lerp(v0_world,v1_world,rmid);
// 	*mid_world = l3d_vecLerp(v0_world, v1_world, rmid);
// }

// 
// labels 18-19
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

		// l3d_drawCircleCallback(v0->x,
		// 						v0->y,
		// 						l3d_floatToRational(SCREEN_WIDTH * 0.005f),
		// 						(l3d_colour_t)L3D_COLOUR_DARKYELLOW);
		// l3d_drawCircleCallback(v1->x,
		// 						v1->y,
		// 						l3d_floatToRational(SCREEN_WIDTH * 0.005f),
		// 						(l3d_colour_t)L3D_COLOUR_DARKYELLOW);

		// l3d_drawCircleCallback(p0.x,
		// 						p0.y,
		// 						l3d_floatToRational(SCREEN_WIDTH * 0.005f),
		// 						(l3d_colour_t)L3D_COLOUR_MAGENTA);

		// l3d_drawCircleCallback(p1.x,
		// 						p1.y,
		// 						l3d_floatToRational(SCREEN_WIDTH * 0.005f),
		// 						(l3d_colour_t)L3D_COLOUR_MAGENTA);

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
	// if (edge_v0_idx == tri_v0_idx && edge_v1_idx == tri_v1_idx ||
	// 	edge_v0_idx == tri_v1_idx && edge_v1_idx == tri_v0_idx ||
	// 	edge_v0_idx == tri_v0_idx && edge_v1_idx == tri_v2_idx ||
	// 	edge_v0_idx == tri_v2_idx && edge_v1_idx == tri_v0_idx ||
	// 	edge_v0_idx == tri_v1_idx && edge_v1_idx == tri_v2_idx ||
	// 	edge_v0_idx == tri_v2_idx && edge_v1_idx == tri_v1_idx
	// )

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
// TODO: move into some other translation unit, not into core as it includes hle
// 
l3d_vec4_t transformVertexIntoViewSpace(const l3d_vec4_t *v_world, const l3d_mat4x4_t *mat_view, const l3d_mat4x4_t *mat_proj) {
#ifdef L3D_CAMERA_MOVABLE
	l3d_vec4_t v_viewed = l3d_mat4x4_mulVec4(mat_view, v_world);
	l3d_vec4_t v_projected = l3d_mat4x4_mulVec4(mat_proj, &v_viewed);
#else
	l3d_vec4_t v_projected = l3d_mat4x4_mulVec4(mat_proj, v_world);
#endif
	// Scale into view, we moved the normalising into cartesian space
	// out of the matrix.vector function from the previous versions, so
	// do this manually:
	// TODO: fix the commented guard
	// if (v_projected.h < L3D_EPSILON_RTNL) {
	// 	L3D_DEBUG_PRINT("Error: Division by zero. Aborting\n");
	// 	return l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);
	// }
	v_projected = l3d_vec4_div(&v_projected, v_projected.h);

	l3d_vec4_t v_offset_view = l3d_getVec4FromFloat(1.0f, 1.0f, 0.0f, 0.0f);

	v_projected = l3d_vec4_add(&v_projected, &v_offset_view);

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
	v_projected.x = l3d_fixedMul(v_projected.x, l3d_floatToFixed(0.5f * (l3d_flp_t)SCREEN_WIDTH));
	v_projected.y = l3d_fixedMul(v_projected.y, l3d_floatToFixed(0.5f * (l3d_flp_t)SCREEN_HEIGHT));
#else
	v_projected.x *= 0.5f * (l3d_flp_t)SCREEN_WIDTH;
	v_projected.y *= 0.5f * (l3d_flp_t)SCREEN_HEIGHT;
#endif

	return v_projected;
}

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
    // 3 iterations are usually enough
    for(uint8_t iter = 0; iter < 3; iter++)
    {
        // 3D point on edge
        l3d_vec4_t p = l3d_vecLerp(world0, world1, t);

        // // // Transform to camera space
        // // // l3d_mat4x4_t view;
        // // // l3d_computeViewMatrix((l3d_camera_t*)camera, &view);

        // // l3d_vec4_t view_p = l3d_mat4x4_mulVec4(mat_view, &p);

        // // // Perspective projection
        // // // l3d_mat4x4_t proj;
        // // // l3d_makeProjectionMatrix(&proj, camera);

        // // l3d_vec4_t clip = l3d_mat4x4_mulVec4(mat_proj, &view_p);

        // // // if(l3d_abs(clip.h) < L3D_EPSILON_RTNL)
        // // //     break;

        // // // Perspective divide
        // // l3d_rtnl_t x = clip.x / clip.h;

		// l3d_vec4_t clip = transformVertexIntoViewSpace(&p, mat_view, mat_proj);

        // // Convert desired screen point to NDC
        // l3d_rtnl_t target_x =
        //     (screen_mid.x / l3d_floatToRational(SCREEN_WIDTH))
        //     * l3d_floatToRational(2.0f)
        //     - l3d_floatToRational(1.0f);

        // // Error in projected x
        // // l3d_rtnl_t error = x - target_x;
		// l3d_rtnl_t error = clip.x - target_x;

		// Compute the error:

		// Project current 3D point to screen pixels
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

		// Compute the derivative:

        // Numerical derivative
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

//         l3d_vec4_t p2 = l3d_vecLerp(world0, world1, t + dt);

//         // l3d_vec4_t view_p2 = l3d_mat4x4_mulVec4(mat_view, &p2);

//         // l3d_vec4_t clip2 = l3d_mat4x4_mulVec4(mat_proj, &view_p2);

// 		l3d_vec4_t clip2 = transformVertexIntoViewSpace(&p2, mat_view, mat_proj);

//         // if(l3d_abs(clip2.h) < L3D_EPSILON_RTNL)
//         //     break;

// #if L3D_USE_FIXED_POINT_ARITHMETIC
// 		l3d_rtnl_t x2 = l3d_fixedDiv(clip2.x, clip2.h);
// 		l3d_rtnl_t deriv = l3d_fixedDiv((x2 - x), dt);
// #else
//         // l3d_rtnl_t x2 = clip2.x / clip2.h;
// 		// l3d_rtnl_t deriv = (x2 - x) / dt;
// 		l3d_rtnl_t deriv = (clip2.x - clip.x) / dt;
// #endif /* L3D_USE_FIXED_POINT_ARITHMETIC */

//         // if(l3d_abs(deriv) < L3D_EPSILON_RTNL)
//         //     break;

        // Newton step
#if L3D_USE_FIXED_POINT_ARITHMETIC
		t = t - l3d_fixedDiv(error, deriv);
#else
        t = t - error / deriv;
#endif

        // Clamp
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

	// For each edge
	for (uint16_t edge_data_idx = 0; edge_data_idx < scene->model_edge_count * 3; edge_data_idx += 3)
	{
		// uint16_t edge_id = edge_data_idx/3;	// used only for e.g. visibility flag check

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

			const l3d_vec4_t *tri_v0_world_p = &(scene->vertices_world[tri_v0_idx]);
			const l3d_vec4_t *tri_v1_world_p = &(scene->vertices_world[tri_v1_idx]);
			const l3d_vec4_t *tri_v2_world_p = &(scene->vertices_world[tri_v2_idx]);

			const l3d_vec4_t *tri_v0_proj_p = &(scene->vertices_projected[tri_v0_idx]);
			const l3d_vec4_t *tri_v1_proj_p = &(scene->vertices_projected[tri_v1_idx]);
			const l3d_vec4_t *tri_v2_proj_p = &(scene->vertices_projected[tri_v2_idx]);

			// if(edge belongs triangle)
			//     continue;
			if (l3d_hle_edgeBelongsToFace(e_v0_idx, e_v1_idx,
										  tri_v0_idx, tri_v1_idx, tri_v2_idx))
			{
				// L3D_DEBUG_PRINT("Edge (%d, %d) belongs to tri (%d, %d, %d). Continuing.\n",
				// 				e_v0_idx, e_v1_idx, tri_v0_idx, tri_v1_idx, tri_v2_idx);
				continue;
			}

			// if(!quickReject())
			//     continue;
			// cases A, B, and C
			// a simple boundingbox overlap check should be sufficient?...
			if (!l3d_hle_bboxOverlap(e_v0_proj_p, e_v1_proj_p,
									 tri_v0_proj_p, tri_v1_proj_p, tri_v2_proj_p))
			{
				// L3D_DEBUG_PRINT("No bbox overlap for edge (%d, %d) and tri (%d, %d, %d). Continuing.\n",
				// 				e_v0_idx, e_v1_idx, tri_v0_idx, tri_v1_idx, tri_v2_idx);
				continue;
			}

			// if(!findOverlap())
			//     continue;
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

			// midpoint()
			l3d_camera_t *cam_p = l3d_scene_getActiveCamera(scene);
#if L3D_USE_FIXED_POINT_ARITHMETIC
			l3d_rtnl_t rmid = l3d_fixedDiv((rmin + rmax), 2);
#else
			l3d_rtnl_t rmid = (rmin + rmax) / 2;
#endif /*  */
			// l3d_vec4_t mid_world = l3d_vecLerp(e_v0_world_p, e_v1_world_p, rmid);
			l3d_rtnl_t world_r;
			l3d_projectEdgeParameter(
				e_v0_world_p, e_v1_world_p,
				e_v0_proj_p, e_v1_proj_p,
				&(scene->mat_view),
				&(scene->mat_proj),
				rmid,
				&world_r);
			
			// L3D_DEBUG_PRINT("rmin = %.3f, rmax = %.3f, rmid = %.3f, world_r = %.3f\n",
			// 	l3d_rationalToFloat(rmin), l3d_rationalToFloat(rmax), l3d_rationalToFloat(rmid), l3d_rationalToFloat(world_r));

			l3d_vec4_t mid_world =
				l3d_vecLerp(e_v0_world_p,
							e_v1_world_p,
							world_r);

			// l3d_vec4_t mid_screen = transformVertexIntoViewSpace(&mid_world,
			// 							&(scene->mat_view), &(scene->mat_proj));

			// l3d_drawCircleCallback(
			// 	l3d_rationalToInt32(mid_screen.x), l3d_rationalToInt32(mid_screen.y),
			// 	5,
			// 	(l3d_colour_t)L3D_COLOUR_RED);				

			// plane()
			l3d_plane_t plane;
			l3d_plane_compute(&plane,
							  tri_v0_world_p, tri_v1_world_p, tri_v2_world_p);
			// Later: move this to some cache storage not to repeat computation for every edge

			// // TODO: double-check order of arguments
			// l3d_vec4_t tri_e1_world = l3d_vec4_sub(tri_v1_world_p, tri_v0_world_p);
			// l3d_vec4_t tri_e2_world = l3d_vec4_sub(tri_v2_world_p, tri_v0_world_p);
			// // TODO: double-check order of arguments
			// l3d_vec4_t plane_normal = l3d_vec4_crossProduct(&tri_e1_world, &tri_e2_world);

			// l3d_rtnl_t dist_edge = l3d_vec4_dotProduct(&plane_normal, &mid_world) - plane.D;
			l3d_rtnl_t dist_edge = l3d_plane_eval(&plane, &mid_world);

			// l3d_rtnl_t dist_cam = l3d_vec4_dotProduct(&plane_normal, &(cam_p->local_pos)) - plane.D;
			l3d_rtnl_t dist_cam = l3d_plane_eval(&plane, &(cam_p->local_pos));
			// why calculate distance using dot product?
			// why compare these distances by sign?

			// visibility()
			if(l3d_sign(dist_edge) == l3d_sign(dist_cam))
			{
				// triangle behind edge
				// L3D_DEBUG_PRINT("Edge (%d, %d) in front of tri (%d, %d, %d). Continuing.\n",
				// 				e_v0_idx, e_v1_idx, tri_v0_idx, tri_v1_idx, tri_v2_idx);
				continue;
			}

			// L3D_DEBUG_PRINT("Edge (%d, %d) behind tri (%d, %d, %d) at (%.3f, %.3f). Subtracting interval.\n",
			// 				e_v0_idx, e_v1_idx, tri_v0_idx, tri_v1_idx, tri_v2_idx,
			// 				l3d_rationalToFloat(rmin), l3d_rationalToFloat(rmax));

			// if ()

			// bool is_e0_in_face = l3d_isPointInTri(e_v0_proj_p,
			// 									tri_v0_proj_p, tri_v1_proj_p, tri_v2_proj_p);
			// bool is_e1_in_face = l3d_isPointInTri(e_v1_proj_p,
			// 									tri_v0_proj_p, tri_v1_proj_p, tri_v2_proj_p);
			
			// if (is_e0_in_face)
			// 	rmin = L3D_RTNL_ZERO;
			

			// subtract()
			// Subtract interval computed above in l3d_hle_findOverlap()
			l3d_interval_subtract(&il, rmin, rmax);
		}

		// drawVisibleIntervals()
		// l3d_drawLineCallback(
		// 		l3d_rationalToInt32(e_v0_proj_p->x), l3d_rationalToInt32(e_v0_proj_p->y),
		// 		l3d_rationalToInt32(e_v1_proj_p->x), l3d_rationalToInt32(e_v1_proj_p->y),
		// 		(l3d_colour_t)L3D_COLOUR_WHITE);
		l3d_drawVisibleIntervals(e_v0_proj_p, e_v1_proj_p,
								 &il,
								 (l3d_colour_t)L3D_COLOUR_WHITE);
	}

	return L3D_OK;
}
