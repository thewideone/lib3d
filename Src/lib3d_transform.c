#include "../Inc/lib3d_transform.h"
#include "../Inc/lib3d_util.h"	// for debug print
#include <assert.h>	// for debug

#ifdef L3D_USE_CLIPPING
// 
// Clip given edge against given plane.
// 
// Return -1 if an error occured,
// Return 0 if edge is fully in front of the plane (no clipping was needed),
// return 1 if first vertex of the edge was clipped,
// return 2 if second vertex of the edge was clipped,
// return 3 if both vertices were clipped (edge is fully in behind the plane).
// 
// plane_normal	- normal of the plane
// plane_point	- point on the plane
// world0		- first vertex of the edge in world space
// world1		- second vertex of the edge in world space
// proj0		- first vertex of the edge in screen space
// proj1		- second vertex of the edge in screen space
// 
uint8_t l3d_clip_edge_against_plane(
    l3d_vec4_t *plane_normal,
	const l3d_vec4_t *plane_point,
    const l3d_vec4_t *world0,
    const l3d_vec4_t *world1,
    l3d_vec4_t *proj0,
    l3d_vec4_t *proj1,
	const l3d_mat4x4_t *mat_view,
	const l3d_mat4x4_t *mat_proj)
{
	// Normalise plane normal
	*plane_normal = l3d_vec4_normalise(plane_normal);

	// Get signed distance from each edge vertex to the near plane

	l3d_rtnl_t dist_world0 = l3d_plane_point_dist(
								plane_normal,
								plane_point,
								world0); // l3d_plane_eval(&plane, world0);
	l3d_rtnl_t dist_world1 = l3d_plane_point_dist(
								plane_normal,
								plane_point,
								world1);

	// If both vertices are in front of the near plane,
	// don't change anything, the edge is fully inside the view frustum
	if (l3d_sign(dist_world0) >= L3D_RTNL_ZERO &&
		l3d_sign(dist_world1) >= L3D_RTNL_ZERO)
	{
		return 0;
	}

	// If both vertices are behind the near plane,
	// the edge is completely clipped (outside the view frustum)
	if (l3d_sign(dist_world0) < L3D_RTNL_ZERO &&
		l3d_sign(dist_world1) < L3D_RTNL_ZERO)
	{
		return 3;
	}

	l3d_vec4_t intersection_point;
	bool intersects = l3d_edge_plane_intersection(
							plane_normal, plane_point,
							world0, world1,
							&intersection_point);

	if (!intersects)
	{
		L3D_DEBUG_PRINT("Warning: Edge does not intersect the plane, but one vertex is in front and the other is behind the plane. This should not happen. Returning.\n");
		return -1;
	}

	// Project the intersection point into screen space
	intersection_point = transformVertexIntoViewSpace(
							&intersection_point,
							mat_view,
							mat_proj);

	// If the first vertex is behind the near plane,
	// adjust first projected vertex to the near plane
	// intersection point
	if (l3d_sign(dist_world0) <  L3D_RTNL_ZERO &&
		l3d_sign(dist_world1) >= L3D_RTNL_ZERO)
	{
		*proj0 = intersection_point;
		return 1;
	}
	// If the second vertex is behind the near plane,
	// adjust second projected vertex to the near plane
	// intersection point
	// else if (l3d_sign(dist_world0) >= L3D_RTNL_ZERO &&
	// 		 l3d_sign(dist_world1) <  L3D_RTNL_ZERO)
	// {
	*proj1 = intersection_point;
	// }
	
	return 2;
}

#endif /* L3D_USE_CLIPPING */

// 
// Raw model data -> object in the scene (in world space)
// 
l3d_err_t l3d_transformObjectIntoWorldSpace(
	l3d_scene_t *scene,
	l3d_obj_type_t type,
	uint16_t idx,
	const l3d_mat4x4_t *mat_world)
{
	l3d_obj3d_t *obj3d = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return L3D_DATA_EMPTY;
			// Transform orientation markers into world space
			cam->u_world[0] = l3d_mat4x4_mulVec4(mat_world, &cam->u[0]);	// TODO?: replace this with l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f) etc.
			cam->u_world[1] = l3d_mat4x4_mulVec4(mat_world, &cam->u[1]);
			cam->u_world[2] = l3d_mat4x4_mulVec4(mat_world, &cam->u[2]);
			cam->u_world[3] = l3d_mat4x4_mulVec4(mat_world, &cam->u[3]);
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return L3D_DATA_EMPTY;
			// Transform all vertices of current object to world space
			uint16_t vert_count = obj3d->mesh.vert_count;
			uint16_t model_vert_data_offset = obj3d->mesh.model_vert_data_offset;
			uint16_t tr_vert_offset = obj3d->mesh.transformed_vertices_offset;

			// L3D_DEBUG_PRINT("obj idx: %d; vert_count = %d, model_vert_data_offset = %d, tr_vert_offset = %d\n",
			// 				idx, vert_count, model_vert_data_offset, tr_vert_offset);
			
			// L3D_DEBUG_PRINT_MAT4X4(mat_world);

			for (uint16_t v_id = 0; v_id < vert_count; v_id++) {
				// Get vertex from vertex data of current object's mesh
				l3d_vec4_t vertex = {
					scene->model_vert_data[model_vert_data_offset + v_id*3 + 0],
					scene->model_vert_data[model_vert_data_offset + v_id*3 + 1],
					scene->model_vert_data[model_vert_data_offset + v_id*3 + 2],
					l3d_floatToRational(1.0f)
				};

				l3d_vec4_t v_world = l3d_mat4x4_mulVec4(mat_world, &vertex);

				scene->vertices_world[tr_vert_offset + v_id] = v_world; // shallow copy is sufficient

				// L3D_DEBUG_PRINT("v_id %d: data idx's: (%d, %d, %d)\n", v_id,
				// 	model_vert_data_offset + v_id*3 + 0, model_vert_data_offset + v_id*3 + 1, model_vert_data_offset + v_id*3 + 2);
				// L3D_DEBUG_PRINT("stored into: scene->vertices_world[%d]\n", tr_vert_offset + v_id);
			}

			// Transform orientation markers into world space
			obj3d->u_world[0] = l3d_mat4x4_mulVec4(mat_world, &obj3d->u[0]);	// TODO?: replace this with l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f) etc.
			obj3d->u_world[1] = l3d_mat4x4_mulVec4(mat_world, &obj3d->u[1]);
			obj3d->u_world[2] = l3d_mat4x4_mulVec4(mat_world, &obj3d->u[2]);
			obj3d->u_world[3] = l3d_mat4x4_mulVec4(mat_world, &obj3d->u[3]);
			break;
		default:
			L3D_DEBUG_PRINT("Error: Unknown object type %d. Returning.\n", (int)type);
			return L3D_WRONG_PARAM;
	}

	return L3D_OK;
}

// 
// Transform given vertex in world space coordinates
// into screen space coordinates.
// 
l3d_vec4_t transformVertexIntoViewSpace(
	const l3d_vec4_t *v_world,
	const l3d_mat4x4_t *mat_view,
	const l3d_mat4x4_t *mat_proj)
{
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

// 
// Transform all vertices of the input array to view space,
// project them onto 2D screen coordinates,
// and put into the output array.
// 
// arr_size - size of both arrays
// 
void transformVertexArrayIntoViewSpace(
	const l3d_vec4_t *input_array,
	l3d_vec4_t *output_array,
	uint16_t arr_size,
	const l3d_mat4x4_t *mat_view,
	const l3d_mat4x4_t *mat_proj)
{
	for (uint16_t v_id = 0; v_id < arr_size; v_id++ ) {
		l3d_vec4_t v_world = input_array[v_id];
#ifdef L3D_CAMERA_MOVABLE
		l3d_vec4_t v_viewed = l3d_mat4x4_mulVec4(mat_view, &v_world);
		l3d_vec4_t v_projected = l3d_mat4x4_mulVec4(mat_proj, &v_viewed);
#else
		l3d_vec4_t v_projected = l3d_mat4x4_mulVec4(mat_proj, &v_world);
#endif
		// Scale into view, we moved the normalising into cartesian space
		// out of the matrix.vector function from the previous versions, so
		// do this manually:
		// TODO: fix the commented guard
		// if (v_projected.h < L3D_EPSILON_RTNL) {
		// 	// L3D_DEBUG_PRINT("Error: Division by zero: v_projected.h = %f. Aborting\n",
		// 	// 				l3d_rationalToFloat(v_projected.h));
		// 	L3D_DEBUG_PRINT("Error: Division by zero:\n");
		// 	L3D_DEBUG_PRINT_MAT4X4_P(mat_view);
		// 	L3D_DEBUG_PRINT_MAT4X4_P(mat_proj);
		// 	L3D_DEBUG_PRINT_VEC4(v_world);
		// 	L3D_DEBUG_PRINT_VEC4(v_viewed);
		// 	L3D_DEBUG_PRINT_VEC4(v_projected);
		// 	return;
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

		// Update the projected vertex
		output_array[v_id] = v_projected;
	}
}

l3d_err_t l3d_transformObjectIntoViewSpace(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	l3d_obj3d_t *obj3d = NULL;
	l3d_char3d_t *char3d = NULL;
	l3d_camera_t *cam_p = NULL;

	l3d_vec4_t near_plane_normal, near_plane_point;
	uint16_t tested_obj_id, tested_obj_edge_offset, tested_obj_edge_count, tested_obj_vert_offset;

	if (scene == NULL)
		return L3D_DATA_EMPTY;

	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			if (idx >= scene->camera_count)
				return L3D_WRONG_PARAM;
			
			// Transforming camera's location marker results in division by 0
			return L3D_INVALID_RESULT;
			
			// l3d_camera_t *cam = &scene->cameras[idx];
			// if (cam == NULL)
			// 	return;
			// Transform orientation markers to view space
			// and project it onto 2D space
			// transformVertexArrayIntoViewSpace(cam->u_world, cam->u_proj, 4, &scene->mat_view, &scene->mat_proj);
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			if (idx >= scene->object_count)
				return L3D_WRONG_PARAM;
			
			obj3d = &scene->objects[idx];
			
			if (obj3d == NULL)
				return L3D_DATA_EMPTY;
			
			// Transform all vertices to view space
			// uint16_t tr_vert_offset = obj3d->mesh.transformed_vertices_offset;
			// l3d_vec4_t *first_v_world_ptr = &scene->vertices_world[tr_vert_offset];
			// l3d_vec4_t *first_v_proj_ptr = &scene->vertices_projected[tr_vert_offset];
			// uint16_t vert_count = obj3d->mesh.vert_count;
			// transformVertexArrayIntoViewSpace(first_v_world_ptr, first_v_proj_ptr, vert_count, &scene->mat_view, &scene->mat_proj);

			// For depth clipping
			cam_p = l3d_scene_getActiveCamera(scene);
			if (cam_p == NULL)
			{
				return L3D_DATA_EMPTY;
			}

			near_plane_normal = l3d_vec4_normalise(&(cam_p->local_look_dir));
			near_plane_point = l3d_vec4_mul(&(cam_p->local_look_dir), cam_p->near_plane);
			near_plane_point = l3d_vec4_add(&near_plane_point, &(cam_p->local_pos));

			// Compute ID and offsets of the object that currently processed edge belongs to
			tested_obj_id = idx;
			tested_obj_edge_offset = scene->objects[tested_obj_id].mesh.model_edge_data_offset / 3;
			// uint16_t tested_obj_face_offset = scene->objects[tested_obj_id].mesh.model_tri_data_offset / 3;
			tested_obj_edge_count  = scene->objects[tested_obj_id].mesh.edge_count;
			tested_obj_vert_offset = scene->objects[tested_obj_id].mesh.model_vert_data_offset / 3;

			// L3D_DEBUG_PRINT("tested_obj_id = %d\n", tested_obj_id);
			// L3D_DEBUG_PRINT("tested_obj_edge_offset = %d\n", tested_obj_edge_offset);
			// L3D_DEBUG_PRINT("tested_obj_face_offset = %d\n", tested_obj_face_offset);
			// L3D_DEBUG_PRINT("tested_obj_vert_offset = %d\n", tested_obj_vert_offset);
			// L3D_DEBUG_PRINT("tested_obj_edge_count = %d\n", tested_obj_edge_count);

			// For each edge of given object
			for (uint16_t edge_data_idx = tested_obj_edge_offset * 3;
				edge_data_idx < tested_obj_edge_offset * 3 + tested_obj_edge_count * 3;
				edge_data_idx += 3)
			{
				// L3D_DEBUG_PRINT("Edge %d:\n", edge_data_idx / 3);

				// Absolute ID!! there are offsets for each instance of each mesh!
				uint16_t edge_id = edge_data_idx / 3;
				uint8_t flags = scene->edge_flags[edge_id];
				// May be added in the future:
				// if (!L3D_IS_EDGE_VISISBLE(flags))
				// 	continue;

				// Recompute offsets for the currently tested object if needed
				if (edge_id >= tested_obj_edge_offset + tested_obj_edge_count)
				{
					L3D_DEBUG_PRINT("Error: edge_id (%d) >= tested_obj_edge_offset (%d) + tested_obj_edge_count (%d)\n",
									edge_id, tested_obj_edge_offset, tested_obj_edge_count);
					return L3D_BUFF_OVF;
					// tested_obj_id++;
					// if (tested_obj_id > scene->object_count)
					// {
					// 	L3D_DEBUG_PRINT("Tested object ID (%d) > number of objects in the scene (%d).",
					// 					tested_obj_id, scene->object_count);
					// 	return L3D_BUFF_OVF;
					// }

					// tested_obj_edge_offset = scene->objects[tested_obj_id].mesh.model_edge_data_offset / 3;
					// tested_obj_edge_count  = scene->objects[tested_obj_id].mesh.edge_count;
					// tested_obj_vert_offset = scene->objects[tested_obj_id].mesh.model_vert_data_offset / 3;

					// L3D_DEBUG_PRINT("tested_obj_id = %d\n", tested_obj_id);
					// L3D_DEBUG_PRINT("tested_obj_edge_offset = %d\n", tested_obj_edge_offset);
					// L3D_DEBUG_PRINT("tested_obj_edge_count = %d\n", tested_obj_edge_count);
					// L3D_DEBUG_PRINT("tested_obj_vert_offset = %d\n", tested_obj_vert_offset);
				}

				const uint16_t e_v0_idx = scene->model_edge_data[edge_data_idx + 0] + tested_obj_vert_offset;
				const uint16_t e_v1_idx = scene->model_edge_data[edge_data_idx + 1] + tested_obj_vert_offset;

				// L3D_DEBUG_PRINT("Edge %d: vertices: (%d, %d):\n", edge_data_idx / 3, e_v0_idx, e_v1_idx);

				const l3d_vec4_t *e_v0_world_p = &(scene->vertices_world[e_v0_idx]);
				const l3d_vec4_t *e_v1_world_p = &(scene->vertices_world[e_v1_idx]);

				l3d_vec4_t e_v0_proj, e_v1_proj;
				// l3d_vec4_t e_v0_proj = transformVertexIntoViewSpace(
				// 							e_v0_world_p,
				// 							&(scene->mat_view),
				// 							&(scene->mat_proj));// = &(scene->vertices_projected[e_v0_idx]);
				// l3d_vec4_t e_v1_proj = transformVertexIntoViewSpace(
				// 							e_v1_world_p,
				// 							&(scene->mat_view),
				// 							&(scene->mat_proj));// = &(scene->vertices_projected[e_v1_idx]);

				// 
				// Perform clipping against the near plane of the view frustum
				// 

				// Test whether the edge is inside the view frustum and clip it if needed
				uint8_t clip_result = l3d_clip_edge_against_plane(
													&near_plane_normal, &near_plane_point,
													e_v0_world_p, e_v1_world_p,
													&e_v0_proj, &e_v1_proj,
													&(scene->mat_view), &(scene->mat_proj));

				// Update the edge flags to indicate whether the edge was clipped or not
				switch (clip_result) {
					case 0:	// edge is fully inside the view frustum
						// L3D_DEBUG_PRINT("Edge %d is fully inside the view frustum.\n", edge_id);
						scene->edge_flags[edge_id] &= ~(1 << L3D_EDGE_FLAG_CLIPPED_BIT);
						scene->edge_flags[edge_id] |= (1 << L3D_EDGE_FLAG_VISIBILITY_BIT);

						e_v0_proj = transformVertexIntoViewSpace(
											e_v0_world_p,
											&(scene->mat_view),
											&(scene->mat_proj));
						e_v1_proj = transformVertexIntoViewSpace(
											e_v1_world_p,
											&(scene->mat_view),
											&(scene->mat_proj));
						break;
					case 1:	// first vertex of the edge was clipped
						// L3D_DEBUG_PRINT("Edge %d first vertex was clipped.\n", edge_id);
						scene->edge_flags[edge_id] |= (1 << L3D_EDGE_FLAG_CLIPPED_BIT) | (1 << L3D_EDGE_FLAG_VISIBILITY_BIT);
						e_v1_proj = transformVertexIntoViewSpace(
											e_v1_world_p,
											&(scene->mat_view),
											&(scene->mat_proj));
						break;
					case 2:	// second vertex of the edge was clipped
						// L3D_DEBUG_PRINT("Edge %d second vertex was clipped.\n", edge_id);
						scene->edge_flags[edge_id] |= (1 << L3D_EDGE_FLAG_CLIPPED_BIT) | (1 << L3D_EDGE_FLAG_VISIBILITY_BIT);
						e_v0_proj = transformVertexIntoViewSpace(
											e_v0_world_p,
											&(scene->mat_view),
											&(scene->mat_proj));
						break;
					case 3:	// both vertices of the edge were clipped
						// L3D_DEBUG_PRINT("Edge %d both vertices were clipped.\n", edge_id);
						scene->edge_flags[edge_id] |= (1 << L3D_EDGE_FLAG_CLIPPED_BIT);
						scene->edge_flags[edge_id] &= ~(1 << L3D_EDGE_FLAG_VISIBILITY_BIT);
						break;
					default:
						L3D_DEBUG_PRINT("Error: Invalid clip result (%d) for edge %d.\n", clip_result, edge_id);
						return L3D_INVALID_RESULT;
						break;
				}
				// if (clip_result == 0)
				// {
				// 	scene->edge_flags[edge_id] &= ~(1 << L3D_EDGE_FLAG_CLIPPED_BIT);
				// }
				// else
				// {
				// 	scene->edge_flags[edge_id] |= (1 << L3D_EDGE_FLAG_CLIPPED_BIT);
				// }

				scene->vertices_projected[e_v0_idx] = e_v0_proj;
				scene->vertices_projected[e_v1_idx] = e_v1_proj;
			}

			// Transform orientation markers to view space
			transformVertexArrayIntoViewSpace(obj3d->u_world, obj3d->u_proj, 4, &scene->mat_view, &scene->mat_proj);
			break;
		default:
			L3D_DEBUG_PRINT("Error: Unknown object type %d. Returning.\n", (int)type);
			return L3D_WRONG_PARAM;
	}

	return L3D_OK;
}

// 
// This function applies given transformation matrix to given object in world space
// 
l3d_err_t l3d_applyTransformMatrix(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_mat4x4_t *mat_transform) {
	if (scene == NULL || mat_transform == NULL)
		return L3D_DATA_EMPTY;
	
	l3d_obj3d_t *obj3d = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			if (idx > scene->camera_count)
				return L3D_WRONG_PARAM;
			
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return L3D_DATA_EMPTY;
			
			// Transform local position
			cam->local_pos = l3d_mat4x4_mulVec4(mat_transform, &cam->local_pos);

			// Transform orientation markers
			cam->u_world[0] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[0]);
			cam->u_world[1] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[1]);
			cam->u_world[2] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[2]);
			cam->u_world[3] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[3]);

			cam->has_moved = true;

			// for each child: transform it... really here or in the caller function?
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			if (idx > scene->object_count)
				return L3D_WRONG_PARAM;
			
			obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return L3D_DATA_EMPTY;
			
			// Transform local position
			obj3d->local_pos = l3d_mat4x4_mulVec4(mat_transform, &obj3d->local_pos);

			// Transform all vertices
			uint16_t vert_count = obj3d->mesh.vert_count;
			// uint16_t model_vert_data_offset = obj3d->mesh.model_vert_data_offset;
			uint16_t tr_vert_offset = obj3d->mesh.transformed_vertices_offset;

			for (uint16_t v_id = 0; v_id < vert_count; v_id++) {
				// Get vertex from vertex data of current object's mesh
				l3d_vec4_t vertex = scene->vertices_world[tr_vert_offset + v_id];

				l3d_vec4_t v_transformed = l3d_mat4x4_mulVec4(mat_transform, &vertex);

				scene->vertices_world[tr_vert_offset + v_id] = v_transformed; // shallow copy is sufficient
			}

			// Transform orientation markers
			obj3d->u_world[0] = l3d_mat4x4_mulVec4(mat_transform, &obj3d->u_world[0]);
			obj3d->u_world[1] = l3d_mat4x4_mulVec4(mat_transform, &obj3d->u_world[1]);
			obj3d->u_world[2] = l3d_mat4x4_mulVec4(mat_transform, &obj3d->u_world[2]);
			obj3d->u_world[3] = l3d_mat4x4_mulVec4(mat_transform, &obj3d->u_world[3]);

			obj3d->updated = true;

			// for each child: transform it... really here or in the caller function?
			break;
		default:
			return L3D_WRONG_PARAM;
	}

	return L3D_OK;
}

// 
// Additive translation
// Instead of multiplying by a matrix, add delta_pos to each vertex position
// 
l3d_err_t l3d_additiveTranslateObject(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
	if (scene == NULL || delta_pos == NULL)
		return L3D_DATA_EMPTY;

	l3d_obj3d_t *obj3d = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			if (idx > scene->camera_count)
				return L3D_WRONG_PARAM;
			
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return L3D_DATA_EMPTY;
			
			// Translate local position
			cam->local_pos = l3d_vec4_add(delta_pos, &cam->local_pos);

			// Translate orientation markers
			cam->u_world[0] = l3d_vec4_add(delta_pos, &cam->u_world[0]);
			cam->u_world[1] = l3d_vec4_add(delta_pos, &cam->u_world[1]);
			cam->u_world[2] = l3d_vec4_add(delta_pos, &cam->u_world[2]);
			cam->u_world[3] = l3d_vec4_add(delta_pos, &cam->u_world[3]);

			cam->has_moved = true;

			// for each child: Translate it... really here or in the caller function?
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			if (idx > scene->object_count)
				return L3D_WRONG_PARAM;
			
			obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return L3D_DATA_EMPTY;
			
			obj3d->local_pos = l3d_vec4_add(delta_pos, &obj3d->local_pos);
			// Translate all vertices
			uint16_t vert_count = obj3d->mesh.vert_count;
			// uint16_t model_vert_data_offset = obj3d->mesh.model_vert_data_offset;
			uint16_t tr_vert_offset = obj3d->mesh.transformed_vertices_offset;

			for (uint16_t v_id = 0; v_id < vert_count; v_id++) {
				// Get vertex from vertex data of current object's mesh
				l3d_vec4_t vertex = scene->vertices_world[tr_vert_offset + v_id];

				l3d_vec4_t v_transformed = l3d_vec4_add(delta_pos, &vertex);

				scene->vertices_world[tr_vert_offset + v_id] = v_transformed; // shallow copy is sufficient
			}

			// Translate orientation markers
			obj3d->u_world[0] = l3d_vec4_add(delta_pos, &obj3d->u_world[0]);
			obj3d->u_world[1] = l3d_vec4_add(delta_pos, &obj3d->u_world[1]);
			obj3d->u_world[2] = l3d_vec4_add(delta_pos, &obj3d->u_world[2]);
			obj3d->u_world[3] = l3d_vec4_add(delta_pos, &obj3d->u_world[3]);

			obj3d->updated = true;

			// for each child: translate it... really here or in the caller function?
			break;
		case L3D_OBJ_TYPE_CHAR3D:
			if (idx > scene->char3d_count)
				return L3D_WRONG_PARAM;
			
			char3d = &scene->chars3d[idx];
			if (char3d == NULL)
				return L3D_DATA_EMPTY;
			
			char3d->obj3d.local_pos = l3d_vec4_add(delta_pos, &char3d->obj3d.local_pos);
			// Translate all vertices
			vert_count = char3d->obj3d.mesh.vert_count;
			// uint16_t model_vert_data_offset = char3d->obj3d.mesh.model_vert_data_offset;

			// Set to zero anyway
			tr_vert_offset = char3d->obj3d.mesh.transformed_vertices_offset;

			for (uint16_t v_id = 0; v_id < vert_count; v_id++) {
				// Get vertex from vertex data of current object's mesh
				l3d_vec4_t vertex = char3d->vertices_world[tr_vert_offset + v_id];

				l3d_vec4_t v_transformed = l3d_vec4_add(delta_pos, &vertex);

				char3d->vertices_world[tr_vert_offset + v_id] = v_transformed; // shallow copy is sufficient
			}

			// Translate orientation markers
			char3d->obj3d.u_world[0] = l3d_vec4_add(delta_pos, &char3d->obj3d.u_world[0]);
			char3d->obj3d.u_world[1] = l3d_vec4_add(delta_pos, &char3d->obj3d.u_world[1]);
			char3d->obj3d.u_world[2] = l3d_vec4_add(delta_pos, &char3d->obj3d.u_world[2]);
			char3d->obj3d.u_world[3] = l3d_vec4_add(delta_pos, &char3d->obj3d.u_world[3]);

			char3d->obj3d.updated = true;

			// for each child: translate it... really here or in the caller function?
			break;
		default:
			return L3D_WRONG_PARAM;
	}

	return L3D_OK;
}

// 
// Rotate object about the origin of the coordinate system
// using quaternion as input rotation description
// 
l3d_err_t l3d_rotateGlobalAboutOriginQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta) {
	// Update object's orientation
	// l3d_quat_t q_delta = l3d_axisAngleToQuat(axis, delta_angle_rad);
	l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	orientation = l3d_quat_mul(&orientation, q_delta);
	orientation = l3d_quat_normalise(&orientation);
	l3d_err_t ret = l3d_scene_setObjectOrientation(scene, type, idx, &orientation);
	if (ret != L3D_OK)
		return ret;

	// Transform the object
	l3d_mat4x4_t mat_rot;
	l3d_quatToRotMat(&mat_rot, q_delta);
	return l3d_applyTransformMatrix(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?
}

// 
// Rotate object about a pivot point
// using quaternion as input rotation description
// 
l3d_err_t l3d_rotateGlobalAboutPivotQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *pivot, const l3d_quat_t *q_delta) {
	// Move the object to the origin (0, 0, 0)
	l3d_vec4_t displacement = *pivot;
	displacement = l3d_vec4_negate(&displacement);
	l3d_err_t ret = l3d_additiveTranslateObject(scene, type, idx, &displacement);
	if (ret != L3D_OK)
		return ret;

	// Rotate the object about the origin
	ret = l3d_rotateGlobalAboutOriginQuat(scene, type, idx, q_delta);
	if (ret != L3D_OK)
		return ret;

	// Move the object to its initial position
	displacement = *pivot;
	return l3d_additiveTranslateObject(scene, type, idx, &displacement);
}

// 
// Rotate object at its position being the pivot point
// using quaternion given in global coordinates
// as input rotation description
// 
l3d_err_t l3d_rotateGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta) {
	l3d_vec4_t displacement = l3d_scene_getObjectLocalPos(scene, type, idx);
	return l3d_rotateGlobalAboutPivotQuat(scene, type, idx, &displacement, q_delta);
}


// 
// Rotate object at its position being the pivot point
// using quaternion given in object coordinates
// as input rotation description
// 
// l3d_err_t l3d_rotateLocalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta) {
// 	// q_final_global = q_delta * q_current_global
// 	// l3d_quat_t current_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
// 	// l3d_quat_t q_delta_global = l3d_quat_mul(q_delta, &current_orientation);
// 	// q_delta_global = l3d_quat_inverse(&q_delta_global);

// 	// q_delta_global = q_delta_local * inv(q_current)
// 	l3d_quat_t current_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
// 	current_orientation = l3d_quat_inverse(&current_orientation);
// 	l3d_quat_t q_delta_global = l3d_quat_mul(&current_orientation, q_delta);
// 	L3D_DEBUG_PRINT_QUAT_P(q_delta);
// 	L3D_DEBUG_PRINT_QUAT(current_orientation);
// 	L3D_DEBUG_PRINT_QUAT(q_delta_global);
	
// 	return l3d_rotateGlobalQuat(scene, type, idx, &q_delta_global);
// }

// 
// Rotate object about the origin of the coordinate system
// using axis-angle given in global coordinates
// as input rotation description
// 
l3d_err_t l3d_rotateGlobalAboutOriginAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad) {
	if (scene == NULL || axis == NULL)
		return L3D_DATA_EMPTY;
	
	// Update object's orientation
	l3d_quat_t q_delta = l3d_axisAngleToQuat(axis, delta_angle_rad);
	// l3d_rotateAboutOriginQuat(scene, type, idx, &q_delta);
	l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	orientation = l3d_quat_mul(&orientation, &q_delta);
	l3d_err_t ret = l3d_scene_setObjectOrientation(scene, type, idx, &orientation);
	if (ret != L3D_OK)
		return ret;

	// Transform the object
	l3d_mat4x4_t mat_rot;
	l3d_mat4x4_makeRot(&mat_rot, axis, delta_angle_rad);
	return l3d_applyTransformMatrix(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?
}

// 
// Rotate object about given pivot
// using axis-angle given in global coordinates
// as input rotation description
// 
l3d_err_t l3d_rotateGlobalAboutPivotAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *pivot, const l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad) {
	if (scene == NULL || pivot == NULL || axis == NULL)
		return L3D_DATA_EMPTY;
	
	// Move the object to the origin (0, 0, 0)
	l3d_vec4_t displacement = *pivot;
	displacement = l3d_vec4_negate(&displacement);
	l3d_err_t ret = l3d_additiveTranslateObject(scene, type, idx, &displacement);
	if (ret != L3D_OK)
		return ret;
	
	// Update object's orientation
	l3d_quat_t q_delta = l3d_axisAngleToQuat(axis, delta_angle_rad);
	l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	orientation = l3d_quat_mul(&orientation, &q_delta);
	orientation = l3d_quat_normalise(&orientation);
	ret = l3d_scene_setObjectOrientation(scene, type, idx, &orientation);
	if (ret != L3D_OK)
		return ret;

	// Rotate the object
	// Transform matrix could be made out of the quaternion q_delta aswell
	// It doesn't seem to be any difference
	l3d_mat4x4_t mat_transform;
	l3d_mat4x4_makeRot(&mat_transform, axis, delta_angle_rad);
	ret = l3d_applyTransformMatrix(scene, type, idx, &mat_transform);
	if (ret != L3D_OK)
		return ret;

	// Move the object to its initial position
	displacement = *pivot;
	return l3d_additiveTranslateObject(scene, type, idx, &displacement);
}

// 
// Rotate object about axis given in global space
// using axis-angle as input rotation description
// 
l3d_err_t l3d_rotateGlobalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t displacement = l3d_scene_getObjectLocalPos(scene, type, idx);
	return l3d_rotateGlobalAboutPivotAxisAngle(scene, type, idx, &displacement, axis, delta_angle_rad);
}

// 
// Rotate object about axis given in object space
// using axis-angle as input rotation description
// 
l3d_err_t l3d_rotateLocalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *local_axis, l3d_rtnl_t delta_angle_rad) {
	// Convert the axis given in object space to global space
	l3d_quat_t q_current = l3d_scene_getObjectOrientation(scene, type, idx);
	q_current = l3d_quat_normalise(&q_current);
	l3d_vec4_t global_axis = l3d_rotateVecByQuat(local_axis, &q_current);
	global_axis = l3d_vec4_normalise(&global_axis);

	// Object could be also transformed by axis-angle to quaternion aswell
	// l3d_quat_t q = l3d_axisAngleToQuat(&global_axis, delta_angle_rad);
	// q = l3d_quat_normalise(&q);
	// return l3d_rotateGlobalQuat(scene, type, idx, &q);

	return l3d_rotateGlobalAxisAngle(scene, type, idx, &global_axis, delta_angle_rad);
}

// 
// Rotate object about global X axis
// about angle in radians
// 
l3d_err_t l3d_rotateGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
	// l3d_quat_t q = l3d_axisAngleToQuat(&axis, delta_angle_rad);
	// q = l3d_quat_normalise(&q);
	// return l3d_rotateGlobalQuat(scene, type, idx, &q);
	return l3d_rotateGlobalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Rotate object about global Y axis
// about angle in radians
// 
l3d_err_t l3d_rotateGlobalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
	// l3d_quat_t q = l3d_axisAngleToQuat(&axis, delta_angle_rad);
	// q = l3d_quat_normalise(&q);
	// return l3d_rotateGlobalQuat(scene, type, idx, &q);
	return l3d_rotateGlobalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Rotate object about global Z axis
// about angle in radians
// 
l3d_err_t l3d_rotateGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
	// l3d_quat_t q = l3d_axisAngleToQuat(&axis, delta_angle_rad);
	// q = l3d_quat_normalise(&q);
	// return l3d_rotateGlobalQuat(scene, type, idx, &q);
	return l3d_rotateGlobalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Rotate object about its local X axis
// about angle in radians
// 
l3d_err_t l3d_rotateLocalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
	return l3d_rotateLocalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Rotate object about its local Y axis
// about angle in radians
// 
l3d_err_t l3d_rotateLocalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
	return l3d_rotateLocalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Rotate object about its local Z axis
// about angle in radians
// 
l3d_err_t l3d_rotateLocalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
	return l3d_rotateLocalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Reset object's orientation (in global coordinates)
// 
l3d_err_t l3d_resetOrientationGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	l3d_quat_t diff = l3d_scene_getObjectOrientation(scene, type, idx);
	diff = l3d_quat_inverse(&diff);
	diff = l3d_quat_normalise(&diff);

	return l3d_rotateGlobalQuat(scene, type, idx, &diff);

	// l3d_quat_t old_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	// l3d_quat_t product = l3d_quat_mul(&diff, &old_orientation);
	// product = l3d_quat_normalise(&product);
	// L3D_DEBUG_PRINT_QUAT(product);

	// assert(product.w - q_new->w < L3D_EPSILON_RTNL && 
	// 	product.x - q_new->x < L3D_EPSILON_RTNL && 
	// 	product.y - q_new->y < L3D_EPSILON_RTNL && 
	// 	product.z - q_new->z < L3D_EPSILON_RTNL);
	
	// l3d_mat4x4_t mat_rot;
	// l3d_quatToRotMat(&mat_rot, &diff);
	// l3d_applyTransformMatrix(scene, type, idx, &mat_rot);

	// // Update object's orientation
	// l3d_quat_t qi = l3d_getIdentityQuat();
	// l3d_scene_setObjectOrientation(scene, type, idx, &qi);
	// return L3D_OK;
}

// 
// Set object's orientation
// to a quaternion given in global coordinates
// 
l3d_err_t l3d_setOrientationGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_new) {
	// Reset object's orientation
	l3d_err_t ret = l3d_resetOrientationGlobal(scene, type, idx);
	if (ret != L3D_OK)
		return ret;
	return l3d_rotateGlobalQuat(scene, type, idx, q_new);
}

// Results in some strange results when combining quaternions
// l3d_err_t l3d_setOrientationGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_new) {
// 	// Transform the object
// 	// Compute multiplicative inverse
// 	// Equation taken from:
// 	// https://stackoverflow.com/a/22167097
// 	// diff * old_orientation == q_new (new_orientation)
// 	// diff = q_new * inverse(old_orientation)
// 	l3d_quat_t old_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
// 	L3D_DEBUG_PRINT_QUAT(old_orientation);
// 	// L3D_DEBUG_PRINT("old_rot norm = %.3f\n", l3d_rationalToFloat(l3d_quat_norm(&old_orientation)));
// 	old_orientation = l3d_quat_inverse(&old_orientation);
// 	// L3D_DEBUG_PRINT_QUAT(old_orientation);
// 	old_orientation = l3d_quat_normalise(&old_orientation);
// 	// L3D_DEBUG_PRINT_QUAT(old_orientation);
// 	l3d_quat_t diff = l3d_quat_mul(q_new, &old_orientation);
// 	// L3D_DEBUG_PRINT_QUAT(diff);
// 	diff = l3d_quat_normalise(&diff);
// 	L3D_DEBUG_PRINT_QUAT(diff);
	
// 	L3D_DEBUG_PRINT_QUAT_P(q_new);

// 	old_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
// 	l3d_quat_t product = l3d_quat_mul(&diff, &old_orientation);
// 	product = l3d_quat_normalise(&product);
// 	L3D_DEBUG_PRINT_QUAT(product);

// 	assert(product.w - q_new->w < L3D_EPSILON_RTNL && 
// 		product.x - q_new->x < L3D_EPSILON_RTNL && 
// 		product.y - q_new->y < L3D_EPSILON_RTNL && 
// 		product.z - q_new->z < L3D_EPSILON_RTNL);
	
// 	l3d_mat4x4_t mat_rot;
// 	l3d_quatToRotMat(&mat_rot, &diff);

// 	l3d_applyTransformMatrix(scene, type, idx, &mat_rot);

// 	// Update object's orientation
// 	l3d_scene_setObjectOrientation(scene, type, idx, q_new);
// 	return L3D_OK;
// }

// 
// Set object's orientation to orientation
// given by axis-angle given in global coordinates
// 
l3d_err_t l3d_setOrientationGlobalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t angle_rad) {
	// l3d_quat_t q = l3d_axisAngleToQuat(axis, angle_rad);
	// q = l3d_quat_normalise(&q);
	// return l3d_setOrientationGlobalQuat(scene, type, idx, &q);

	// Seems to be simpler this way - need to test it 
	l3d_err_t ret = l3d_resetOrientationGlobal(scene, type, idx);
	if (ret != L3D_OK)
		return ret;
	return l3d_rotateGlobalAxisAngle(scene, type, idx, axis, angle_rad);
}

// 
// Set object's orientation
// to Euler angles given in global space
// 
l3d_err_t l3d_setOrientationGlobalEuler(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_rot_t *r) {
	l3d_quat_t q = l3d_eulerToQuat(r);
	q = l3d_quat_normalise(&q);
	return l3d_setOrientationGlobalQuat(scene, type, idx, &q);
}

// 
// Move object by a vector given in global coordinates
// 
l3d_err_t l3d_moveGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
	return l3d_additiveTranslateObject(scene, type, idx, delta_pos);
}

// 
// Move object in global X axis by given distance
// 
l3d_err_t l3d_moveGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.x = delta_x;
	return l3d_additiveTranslateObject(scene, type, idx, &delta_pos);
}

// 
// Move object in global Y axis by given distance
// 
l3d_err_t l3d_moveGlobalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_y) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.y = delta_y;
	return l3d_additiveTranslateObject(scene, type, idx, &delta_pos);
}

// 
// Move object in global Z axis by given distance
// 
l3d_err_t l3d_moveGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_z) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.z = delta_z;
	return l3d_additiveTranslateObject(scene, type, idx, &delta_pos);
}

// 
// Move object in its local coordinates
// by a vector given in its local coordinates
// 
l3d_err_t l3d_moveLocal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
	// Get local direction unit vectors
	l3d_vec4_t v_displ_x = l3d_scene_getObjectLocalUnitVecX(scene, type, idx);
	l3d_vec4_t v_displ_y = l3d_scene_getObjectLocalUnitVecY(scene, type, idx);
	l3d_vec4_t v_displ_z = l3d_scene_getObjectLocalUnitVecZ(scene, type, idx);

	// Scale them by corresponding components of the position delta
	v_displ_x = l3d_vec4_mul(&v_displ_x, delta_pos->x);
	v_displ_y = l3d_vec4_mul(&v_displ_y, delta_pos->y);
	v_displ_z = l3d_vec4_mul(&v_displ_z, delta_pos->z);

	// Combine (add) displacement vectors
	l3d_vec4_t v_displacement = v_displ_x;
	v_displacement = l3d_vec4_add(&v_displacement, &v_displ_y);
	v_displacement = l3d_vec4_add(&v_displacement, &v_displ_z);

	return l3d_additiveTranslateObject(scene, type, idx, &v_displacement);
}

// 
// Move object in its local X axis by given distance
// 
l3d_err_t l3d_moveLocalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.x = delta_x;
	return l3d_moveLocal(scene, type, idx, &delta_pos);
}

// 
// Move object in its local Y axis by given distance
// 
l3d_err_t l3d_moveLocalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_y) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.y = delta_y;
	return l3d_moveLocal(scene, type, idx, &delta_pos);
}

// 
// Move object in its local Z axis by given distance
// 
l3d_err_t l3d_moveLocalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_z) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.z = delta_z;
	return l3d_moveLocal(scene, type, idx, &delta_pos);
}

// 
// Set object's global position
// 
l3d_err_t l3d_setGlobalPos(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *dest) {
	l3d_vec4_t current_pos = l3d_scene_getObjectLocalPos(scene, type, idx);
	l3d_vec4_t displacement = l3d_vec4_sub(dest, &current_pos);

	return l3d_additiveTranslateObject(scene, type, idx, &displacement);
}