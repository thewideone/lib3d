#include "../Inc/lib3d_core.h"
#include "../Inc/lib3d_util.h"	// for L3D_DEBUG_PRINT()

l3d_vec4_t global_axes_world[4]; // origin, X, Y, Z
l3d_vec4_t global_axes_proj[4]; // origin, X, Y, Z

void l3d_initGlobalAxesMarker(void) {
	global_axes_world[0] = l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);	// origin
	global_axes_world[1] = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);	// X
	global_axes_world[2] = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);	// Y
	global_axes_world[3] = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);	// Z
}

// 
// Set up projection matrix for mesh transformation.
// mat	- matrix to be set up as a projection mat
// 
void l3d_makeProjectionMatrix(l3d_mat4x4_t *mat, const l3d_camera_t *cam){
	l3d_mat4x4_makeProjection(
			mat,
			cam->fov,
			l3d_floatToRational( (l3d_flp_t)SCREEN_HEIGHT / (l3d_flp_t)SCREEN_WIDTH ),
			cam->near_plane,
			cam->far_plane );
}

#ifdef L3D_CAMERA_MOVABLE
// 
// Compute camera and view matrices,
// the latter of which is used for mesh transformation,
// if using camera.
// cam				- camera
// mat_view			- view matrix
// f_elapsed_time	- elapsed time since the last frame,
// 					  don't ask me about the unit XD
// 
void l3d_computeViewMatrix( l3d_camera_t *cam, l3d_mat4x4_t *mat_view, l3d_flp_t f_elapsed_time ){
	// We've integrated time into this, so it's a velocity vector:
	// DEBUG_PRINT( "cam->look_dir: " );
	// l3d_printVec4( &(cam->look_dir), 1 );

    l3d_vec4_t v_forward = l3d_vec4_mul( &(cam->local_look_dir), l3d_floatToRational( 0.01f * f_elapsed_time ) );
	// l3d_vec4_t v_up = l3d_getZeroVec4();
	// v_up.z = l3d_floatToRational(1.0f);		// Z-axis points up
	l3d_vec4_t v_target = l3d_getZeroVec4();
	v_target.y = l3d_floatToRational(1.0f);	// Y-axis points forward

	l3d_mat4x4_t mat_cam_rot;

	// Apply camera roll: (not working yet)
	// matrix_makeRotZ( &mat_camera_rot, cam->roll );
	// v_up = matrix_mulVector( &mat_camera_rot, &v_up );
	
	// Apply camera pitch and yaw (X -> Z -> Y, order matters):
	// change it to Z->X->Y
	// matrix_makeRotZ( &mat_camera_rot, cam->roll );
	// cam->look_dir = matrix_mulVector( &mat_camera_rot, &v_target );
	l3d_mat4x4_makeRotX( &mat_cam_rot, cam->local_rot.pitch );
    cam->local_look_dir = l3d_mat4x4_mulVec4( &mat_cam_rot, &v_target );
	l3d_mat4x4_makeRotZ( &mat_cam_rot, cam->local_rot.yaw );	// was rotY
	cam->local_look_dir = l3d_mat4x4_mulVec4( &mat_cam_rot, &cam->local_look_dir );

	v_target = l3d_vec4_add( &(cam->local_pos), &(cam->local_look_dir) );

    l3d_mat4x4_t mat_camera;
	// matrix_pointAt( &mat_camera, &(cam->pos), &v_target, &v_up );
	// l3d_mat4x4_pointAt( &mat_camera, &(cam->local_pos), &v_target, &v_up );
	l3d_mat4x4_pointAt( &mat_camera, &(cam->local_pos), &v_target, &(cam->local_up_dir) );

    // Make view matrix from camera:
    l3d_mat4x4_quickInverse( mat_view, &mat_camera );
}
#endif

// 
// Raw model data -> object in the scene
// 
void l3d_transformObjectIntoWorldSpace(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_mat4x4_t *mat_world) {
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			l3d_camera_t *cam = &scene->cameras[idx];
			if (cam == NULL)
				return;
			// Transform orientation markers into world space
			cam->u_world[0] = l3d_mat4x4_mulVec4(mat_world, &cam->u[0]);
			cam->u_world[1] = l3d_mat4x4_mulVec4(mat_world, &cam->u[1]);
			cam->u_world[2] = l3d_mat4x4_mulVec4(mat_world, &cam->u[2]);
			cam->u_world[3] = l3d_mat4x4_mulVec4(mat_world, &cam->u[3]);
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return;
			// Transform all vertices of current object to world space
			uint16_t vert_count = obj3d->mesh.vert_count;
			uint16_t model_vert_data_offset = obj3d->mesh.model_vert_data_offset;
			uint16_t tr_vert_offset = obj3d->mesh.transformed_vertices_offset;

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
			}

			// Transform orientation markers into world space
			obj3d->u_world[0] = l3d_mat4x4_mulVec4(mat_world, &obj3d->u[0]);
			obj3d->u_world[1] = l3d_mat4x4_mulVec4(mat_world, &obj3d->u[1]);
			obj3d->u_world[2] = l3d_mat4x4_mulVec4(mat_world, &obj3d->u[2]);
			obj3d->u_world[3] = l3d_mat4x4_mulVec4(mat_world, &obj3d->u[3]);
			break;
	}
}

// 
// Transform all vertices of the input array to view space,
// project them onto 2D screen coordinates,
// and put into the output array.
// 
// arr_size - size of both arrays
// 
void transformIntoViewSpace(const l3d_vec4_t *input_array, l3d_vec4_t *output_array, uint16_t arr_size, const l3d_mat4x4_t *mat_view, const l3d_mat4x4_t *mat_proj) {
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

void l3d_transformObjectIntoViewSpace(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_mat4x4_t *mat_view, const l3d_mat4x4_t *mat_proj) {
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			// Transforming camera's location marker results in division by 0
			
			// l3d_camera_t *cam = &scene->cameras[idx];
			// if (cam == NULL)
			// 	return;
			// Transform orientation markers to view space
			// and project it onto 2D space
			// transformIntoViewSpace(cam->u_world, cam->u_proj, 4, mat_view, mat_proj);
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return;
			uint16_t vert_count = obj3d->mesh.vert_count;
			// uint16_t model_vert_data_offset = obj3d->mesh.model_vert_data_offset;
			uint16_t tr_vert_offset = obj3d->mesh.transformed_vertices_offset;
			// Transform all vertices to view space
			// and project them onto 2D screen coordinates
			transformIntoViewSpace(scene->vertices_world + tr_vert_offset*sizeof(l3d_vec4_t), scene->vertices_projected, vert_count, mat_view, mat_proj);
			// Transform orientation markers to view space
			// and project it onto 2D space
			transformIntoViewSpace(obj3d->u_world, obj3d->u_proj, 4, mat_view, mat_proj);
			break;
	}
}

void l3d_transformGlobalAxesMarkerIntoViewSpace(const l3d_mat4x4_t *mat_view, const l3d_mat4x4_t *mat_proj) {
	transformIntoViewSpace(global_axes_world, global_axes_proj, 4, mat_view, mat_proj);
}

l3d_err_t l3d_setupObjects(l3d_scene_t *scene, const l3d_mat4x4_t *mat_proj, const l3d_mat4x4_t *mat_view) {
	if (scene == NULL)
		return L3D_WRONG_PARAM;
	// Process each object's vertices
	l3d_obj3d_t *obj3d = NULL;
	l3d_camera_t *cam = NULL;
	l3d_mat4x4_t mat_rot_x, mat_rot_y, mat_rot_z, mat_trans, mat_world, mat_tmp, mat_tmp2;

	for (uint16_t cam_id = 0; cam_id < scene->camera_count; cam_id++) {
		cam = &(scene->cameras[cam_id]);
		if (cam == NULL)
			return L3D_DATA_EMPTY;

		// Rotate the object
		// l3d_rot_t local_rot = l3d_scene_getObjectLocalRot(scene, type, idx);
		l3d_mat4x4_makeRotZ(&mat_rot_z, cam->local_rot.yaw);
		l3d_mat4x4_makeRotY(&mat_rot_y, cam->local_rot.roll);
		l3d_mat4x4_makeRotX(&mat_rot_x, cam->local_rot.pitch);
		// Translate the object
		l3d_mat4x4_makeTranslation(&mat_trans, &cam->local_pos);
		// Make world matrix
		l3d_mat4x4_makeIdentity(&mat_world);
		l3d_mat4x4_makeIdentity(&mat_tmp);

		l3d_mat4x4_mulMatrix(&mat_tmp, &mat_rot_z, &mat_rot_y);
		l3d_mat4x4_mulMatrix(&mat_tmp2, &mat_tmp, &mat_rot_x);
		l3d_mat4x4_mulMatrix(&mat_world, &mat_tmp2, &mat_trans);
		l3d_transformObjectIntoWorldSpace(scene, L3D_OBJ_TYPE_CAMERA, cam_id, &mat_world);
	}
	
	for (uint16_t obj_id = 0; obj_id < scene->object_count; obj_id++) {
		obj3d = &(scene->objects[obj_id]);
		if (obj3d == NULL)
			return L3D_DATA_EMPTY;
		
		// Rotate the object
		// l3d_rot_t local_rot = l3d_scene_getObjectLocalRot(scene, type, idx);
		l3d_mat4x4_makeRotZ(&mat_rot_z, obj3d->local_rot.yaw);
		l3d_mat4x4_makeRotY(&mat_rot_y, obj3d->local_rot.roll);
		l3d_mat4x4_makeRotX(&mat_rot_x, obj3d->local_rot.pitch);
		// Translate the object
		l3d_mat4x4_makeTranslation(&mat_trans, &obj3d->local_pos);
		// Make world matrix
		l3d_mat4x4_makeIdentity(&mat_world);
		l3d_mat4x4_makeIdentity(&mat_tmp);

		l3d_mat4x4_mulMatrix(&mat_tmp, &mat_rot_z, &mat_rot_y);
		l3d_mat4x4_mulMatrix(&mat_tmp2, &mat_tmp, &mat_rot_x);
		l3d_mat4x4_mulMatrix(&mat_world, &mat_tmp2, &mat_trans);

		// l3d_mat4x4_t mat_rot;
		// l3d_vec4_t n;
		// l3d_err_t ret = L3D_OK;

		// // Z
		// n = l3d_getVec4FromFloat(1.0f, 1.0f, 1.0f, 1.0f);
		// n = l3d_vec4_normalise(&n);

		// // ret = l3d_mat4x4_makeRot(&mat_rot, &n, obj3d->local_rot.yaw, &obj3d->local_pos);
		// // ret = l3d_mat4x4_makeRot(&mat_rot, &n, obj3d->local_rot.yaw);

		// if (ret != L3D_OK) {
		// 	L3D_DEBUG_PRINT("l3d_mat4x4_makeRot ret = %d; aborting.\n", ret);
		// 	return ret;
		// }

		// l3d_mat4x4_mulMatrix(&mat_world, &mat_rot, &mat_trans);

		// Now first rotate the object according to its local_rot,
		// and then translate it according to its local_pos

		l3d_transformObjectIntoWorldSpace(scene, L3D_OBJ_TYPE_OBJ3D, obj_id, &mat_world);
		
		// l3d_transformObjectIntoViewSpace(scene, obj3d, mat_view, mat_proj);
	}

	return L3D_OK;
}

// 
// No backface culling yet,
// just to see if bare minimum program works
// 
// This function updates objects in given scene
// 
// Not used yet, to be rewritten
// 
// l3d_err_t l3d_processObjects(l3d_scene_t *scene, const l3d_mat4x4_t *mat_proj, const l3d_mat4x4_t *mat_view) {
// 	if (scene == NULL)
// 		return L3D_WRONG_PARAM;
// 	// Process each object's vertices
// 	l3d_obj3d_t *obj3d = NULL;
// 	l3d_mat4x4_t mat_rot_x, mat_rot_y, mat_rot_z, mat_trans, mat_world, mat_tmp, mat_tmp2;
	
// 	for (uint16_t obj_id = 0; obj_id < scene->object_count; obj_id++) {
// 		obj3d = &(scene->objects[obj_id]);
// 		if (obj3d == NULL)
// 			return L3D_DATA_EMPTY;
		
// 		// Rotate the object
// 		l3d_mat4x4_makeRotZ(&mat_rot_z, obj3d->local_rot.yaw);
// 		l3d_mat4x4_makeRotY(&mat_rot_y, obj3d->local_rot.roll);
// 		l3d_mat4x4_makeRotX(&mat_rot_x, obj3d->local_rot.pitch);
// 		// Translate the object
// 		l3d_mat4x4_makeTranslation(&mat_trans, &obj3d->local_pos);
// 		// Make world matrix
// 		l3d_mat4x4_makeIdentity(&mat_world);
// 		l3d_mat4x4_makeIdentity(&mat_tmp);

// 		// l3d_mat4x4_mulMatrix(&mat_tmp, &mat_rot_z, &mat_rot_y);
// 		// l3d_mat4x4_mulMatrix(&mat_tmp2, &mat_tmp, &mat_rot_x);
// 		// l3d_mat4x4_mulMatrix(&mat_world, &mat_tmp2, &mat_trans);

// 		l3d_mat4x4_t mat_rot;
// 		l3d_vec4_t n;
// 		l3d_err_t ret = L3D_OK;

// 		// Z
// 		n = l3d_getVec4FromFloat(1.0f, 1.0f, 1.0f, 1.0f);
// 		n = l3d_vec4_normalise(&n);

// 		// ret = l3d_mat4x4_makeRot(&mat_rot, &n, obj3d->local_rot.yaw, &obj3d->local_pos);
// 		ret = l3d_mat4x4_makeRot(&mat_rot, &n, obj3d->local_rot.yaw);

// 		if (ret != L3D_OK) {
// 			L3D_DEBUG_PRINT("l3d_mat4x4_makeRot ret = %d; aborting.\n", ret);
// 			return ret;
// 		}
// 		// n = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
// 		// ret = l3d_mat4x4_makeRotGeneral(&mat_rot, &n, &(obj3d->local_pos), obj3d->local_rot.yaw);
// 		// // Y
// 		// n = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
// 		// ret = l3d_mat4x4_makeRot(&mat_rot, &n, obj3d->local_rot.roll);
// 		// n = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
// 		// l3d_mat4x4_makeRotGeneral(&mat_tmp, &n, &(obj3d->local_pos), obj3d->local_rot.roll);
// 		// // X
// 		// n = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
// 		// ret = l3d_mat4x4_makeRot(&mat_rot, &n, obj3d->local_rot.pitch);
// 		// n = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
// 		// l3d_mat4x4_makeRotGeneral(&mat_tmp2, &n, &(obj3d->local_pos), obj3d->local_rot.pitch);

// 		// l3d_mat4x4_mulMatrix(&mat_rot, &mat_rot, &mat_tmp);
// 		// l3d_mat4x4_mulMatrix(&mat_rot, &mat_rot, &mat_tmp2);

// 		l3d_mat4x4_mulMatrix(&mat_world, &mat_rot, &mat_trans);

// 		l3d_transformObjectIntoWorldSpace(scene, obj3d, &mat_world);
		
// 		l3d_transformObjectIntoViewSpace(scene, obj3d, mat_view, mat_proj);
// 	}

// 	return L3D_OK;
// }

l3d_err_t l3d_drawWireframe(const l3d_scene_t *scene, uint16_t obj_id) {
	l3d_obj3d_t *obj3d = &(scene->objects[obj_id]);
	if (obj3d == NULL)
		return L3D_DATA_EMPTY;

	uint16_t edge_data_offset = obj3d->mesh.model_edge_data_offset * 3;

	// For each edge of the object's mesh
	for (uint16_t edge_data_idx = edge_data_offset; edge_data_idx < edge_data_offset + scene->model_edge_count * 3; edge_data_idx += 3) {
		// If edge invisible: continue
		uint16_t edge_id = edge_data_idx/3;
		uint8_t flags = scene->edge_flags[edge_id];
		if (!L3D_IS_EDGE_VISISBLE(flags))
			continue;

		// Get projected vertices
		uint16_t v1_id = scene->model_edge_data[edge_data_idx+0];
		uint16_t v2_id = scene->model_edge_data[edge_data_idx+1];
		// uint16_t tri_id = scene->model_edge_data[edge_id+2];

		l3d_vec4_t v1 = scene->vertices_projected[v1_id];
		l3d_vec4_t v2 = scene->vertices_projected[v2_id];

		// L3D_DEBUG_PRINT("v1: (%.3f, %.3f, %.3f),\tv2: (%.3f, %.3f, %.3f)\r\n",
		// 		l3d_rationalToFloat(v1.x), l3d_rationalToFloat(v1.y), l3d_rationalToFloat(v1.z),
		// 		l3d_rationalToFloat(v2.x), l3d_rationalToFloat(v2.y), l3d_rationalToFloat(v2.z));

		// Draw the edge
#ifdef L3D_DEBUG_EDGES
		if (L3D_IS_EDGE_BOUNDARY(flags))
			l3d_drawLineCallback(
				l3d_rationalToInt32(v1.x), l3d_rationalToInt32(v1.y),
				l3d_rationalToInt32(v2.x), l3d_rationalToInt32(v2.y),
				L3D_DEBUG_BOUNDARY_EDGE_COLOUR);
		else if (L3D_IS_EDGE_SILHOUETTE(flags))
			l3d_drawLineCallback(
				l3d_rationalToInt32(v1.x), l3d_rationalToInt32(v1.y),
				l3d_rationalToInt32(v2.x), l3d_rationalToInt32(v2.y),
				L3D_DEBUG_SILHOUETTE_EDGE_COLOUR);
		else
			l3d_drawLineCallback(
				l3d_rationalToInt32(v1.x), l3d_rationalToInt32(v1.y),
				l3d_rationalToInt32(v2.x), l3d_rationalToInt32(v2.y),
				L3D_DEBUG_VISIBLE_EDGE_COLOUR);
#else
		l3d_drawLineCallback(
			l3d_rationalToInt32(v1.x), l3d_rationalToInt32(v1.y),
			l3d_rationalToInt32(v2.x), l3d_rationalToInt32(v2.y),
			obj3d->wireframe_colour);
#endif	// L3D_DEBUG_EDGES
	}

	return L3D_OK;
}

l3d_err_t l3d_drawGizmos(const l3d_scene_t *scene, uint16_t obj_id) {
	l3d_obj3d_t *obj3d = &(scene->objects[obj_id]);
	if (obj3d == NULL || obj3d->u_proj == NULL)
		return L3D_DATA_EMPTY;
	
	// l3d_vec4_t arrow_x = l3d_vec4_sub(&(obj3d->u_proj[1]), &(obj3d->u_proj[0]));
	// l3d_vec4_t arrow_y = l3d_vec4_sub(&(obj3d->u_proj[2]), &(obj3d->u_proj[0]));
	// l3d_vec4_t arrow_z = l3d_vec4_sub(&(obj3d->u_proj[3]), &(obj3d->u_proj[0]));

	// X
	l3d_drawLineCallback(
		l3d_rationalToInt32(obj3d->u_proj[1].x), l3d_rationalToInt32(obj3d->u_proj[1].y),
		l3d_rationalToInt32(obj3d->u_proj[0].x), l3d_rationalToInt32(obj3d->u_proj[0].y),
		L3D_COLOUR_RED);
	// Y
	l3d_drawLineCallback(
		l3d_rationalToInt32(obj3d->u_proj[2].x), l3d_rationalToInt32(obj3d->u_proj[2].y),
		l3d_rationalToInt32(obj3d->u_proj[0].x), l3d_rationalToInt32(obj3d->u_proj[0].y),
		L3D_COLOUR_GREEN);
	// Z
	l3d_drawLineCallback(
		l3d_rationalToInt32(obj3d->u_proj[3].x), l3d_rationalToInt32(obj3d->u_proj[3].y),
		l3d_rationalToInt32(obj3d->u_proj[0].x), l3d_rationalToInt32(obj3d->u_proj[0].y),
		L3D_COLOUR_BLUE);
	
	return L3D_OK;
}

l3d_err_t l3d_drawGlobalAxesMarker(void) {
	// X
	l3d_drawLineCallback(
		l3d_rationalToInt32(global_axes_proj[1].x), l3d_rationalToInt32(global_axes_proj[1].y),
		l3d_rationalToInt32(global_axes_proj[0].x), l3d_rationalToInt32(global_axes_proj[0].y),
		L3D_COLOUR_RED);
	// Y
	l3d_drawLineCallback(
		l3d_rationalToInt32(global_axes_proj[2].x), l3d_rationalToInt32(global_axes_proj[2].y),
		l3d_rationalToInt32(global_axes_proj[0].x), l3d_rationalToInt32(global_axes_proj[0].y),
		L3D_COLOUR_GREEN);
	// Z
	l3d_drawLineCallback(
		l3d_rationalToInt32(global_axes_proj[3].x), l3d_rationalToInt32(global_axes_proj[3].y),
		l3d_rationalToInt32(global_axes_proj[0].x), l3d_rationalToInt32(global_axes_proj[0].y),
		L3D_COLOUR_BLUE);
	
	return L3D_OK;
}

l3d_err_t l3d_drawObjects(const l3d_scene_t *scene) {
	if (scene == NULL ||
		(scene->object_count > 0 && scene->objects == NULL) ||
		scene->edge_flags == NULL ||
		scene->model_edge_data == NULL ||
		scene->vertices_projected == NULL )
		return L3D_WRONG_PARAM;

	l3d_err_t ret = L3D_OK;

	for (uint16_t obj_id = 0; obj_id < scene->object_count; obj_id++) {
		ret = l3d_drawWireframe(scene, obj_id);

		if (ret != L3D_OK)
			break;
		
		ret = l3d_drawGizmos(scene, obj_id);

		if (ret != L3D_OK)
			break;
	}

	return ret;
}

// 
// For now simply draw all the objects
// 
// Not used yet, to be rewritten
// 
// l3d_err_t l3d_processScene(l3d_scene_t *scene, l3d_flp_t elapsed_time) {
// 	if (scene == NULL)
// 		return L3D_WRONG_PARAM;

// 	// Projection matrix computed in setup
// 	// l3d_makeProjectionMatrix(&(scene->mat_proj), scene->active_camera);
// 	l3d_computeViewMatrix(scene->active_camera, &(scene->mat_view), elapsed_time );
	
// 	l3d_err_t ret;
// 	ret = l3d_processObjects(scene, &(scene->mat_proj), &(scene->mat_view));

// 	if (ret != L3D_OK)
// 		return ret;

// 	// Draw each object
// 	// - draw its edges
// 	ret = l3d_drawObjects(scene);

// 	return ret;
// }
