#include "../Inc/lib3d_core.h"
#include "../Inc/lib3d_util.h"	// for L3D_DEBUG_PRINT()

l3d_vec4_t global_axes_world[4]; // origin, X, Y, Z
l3d_vec4_t global_axes_proj[4]; // origin, X, Y, Z

// 
// Init the marker of the origin of the coordinate system
// 
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
// Compute camera matrix
// cam				- camera
// mat_view			- view matrix
// 
void l3d_computeViewMatrix( l3d_camera_t *cam, l3d_mat4x4_t *mat_view ){
	cam->orientation = l3d_quat_normalise(&cam->orientation);

	l3d_mat4x4_t mat_cam_rot;
	l3d_quatToRotMat(&mat_cam_rot, &cam->orientation);
	
	l3d_vec4_t v_target = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f); // Y-axis points forward
	cam->local_look_dir = l3d_mat4x4_mulVec4( &mat_cam_rot, &v_target );

	l3d_vec4_t up_dir = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);	// Z axis points up
	l3d_quatToRotMat(&mat_cam_rot, &cam->orientation);
	cam->local_up_dir = l3d_mat4x4_mulVec4( &mat_cam_rot, &up_dir );

    l3d_mat4x4_t mat_camera;
	v_target = l3d_vec4_add( &(cam->local_pos), &(cam->local_look_dir) );
	// l3d_mat4x4_pointAt( &mat_camera, &(cam->local_pos), &v_target, &v_up );
	l3d_mat4x4_pointAt( &mat_camera, &(cam->local_pos), &v_target, &(cam->local_up_dir) );

    // Make view matrix from camera:
    l3d_mat4x4_quickInverse( mat_view, &mat_camera );
}
#else
void l3d_computeViewMatrix( l3d_camera_t *cam, l3d_mat4x4_t *mat_view ) {

}
#endif

// 
// Raw model data -> object in the scene (in world space)
// 
void l3d_transformObjectIntoWorldSpace(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_mat4x4_t *mat_world) {
	l3d_obj3d_t *obj3d = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return;
			// Transform orientation markers into world space
			cam->u_world[0] = l3d_mat4x4_mulVec4(mat_world, &cam->u[0]);	// TODO?: replace this with l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f) etc.
			cam->u_world[1] = l3d_mat4x4_mulVec4(mat_world, &cam->u[1]);
			cam->u_world[2] = l3d_mat4x4_mulVec4(mat_world, &cam->u[2]);
			cam->u_world[3] = l3d_mat4x4_mulVec4(mat_world, &cam->u[3]);
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return;
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
	}
}

// 
// Transform all vertices of the input array to view space,
// project them onto 2D screen coordinates,
// and put into the output array.
// 
// arr_size - size of both arrays
// 
void transformVertexArrayIntoViewSpace(const l3d_vec4_t *input_array, l3d_vec4_t *output_array, uint16_t arr_size, const l3d_mat4x4_t *mat_view, const l3d_mat4x4_t *mat_proj) {
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
		if (v_projected.h == l3d_floatToRational(0.0f)) {
			L3D_DEBUG_PRINT("Error: Division by zero. Aborting\n");
			return;
		}
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

void l3d_transformObjectIntoViewSpace(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	l3d_obj3d_t *obj3d = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			// Transforming camera's location marker results in division by 0
			return;
			
			// l3d_camera_t *cam = &scene->cameras[idx];
			// if (cam == NULL)
			// 	return;
			// Transform orientation markers to view space
			// and project it onto 2D space
			// transformVertexArrayIntoViewSpace(cam->u_world, cam->u_proj, 4, &scene->mat_view, &scene->mat_proj);
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			obj3d = &scene->objects[idx];
			
			if (obj3d == NULL)
				return;
			
			// Transform all vertices to view space
			uint16_t tr_vert_offset = obj3d->mesh.transformed_vertices_offset;
			l3d_vec4_t *first_v_world_ptr = &scene->vertices_world[tr_vert_offset];
			l3d_vec4_t *first_v_proj_ptr = &scene->vertices_projected[tr_vert_offset];
			uint16_t vert_count = obj3d->mesh.vert_count;
			transformVertexArrayIntoViewSpace(first_v_world_ptr, first_v_proj_ptr, vert_count, &scene->mat_view, &scene->mat_proj);

			// Transform orientation markers to view space
			transformVertexArrayIntoViewSpace(obj3d->u_world, obj3d->u_proj, 4, &scene->mat_view, &scene->mat_proj);
			break;
	}
}

void l3d_transformGlobalAxesMarkerIntoViewSpace(const l3d_mat4x4_t *mat_view, const l3d_mat4x4_t *mat_proj) {
	transformVertexArrayIntoViewSpace(global_axes_world, global_axes_proj, 4, mat_view, mat_proj);
}

// 
// Setup objects in given scene
// (transform them into world space)
// 
l3d_err_t l3d_setupObjects(l3d_scene_t *scene) {
	if (scene == NULL)
		return L3D_WRONG_PARAM;
	
	// Process each object's vertices
	l3d_obj3d_t *obj3d = NULL;
	l3d_camera_t *cam = NULL;
	l3d_mat4x4_t mat_trans, mat_rot, mat_world;

	// Cameras
	for (uint16_t cam_id = 0; cam_id < scene->camera_count; cam_id++) {
		cam = &(scene->cameras[cam_id]);
		if (cam == NULL)
			return L3D_DATA_EMPTY;

		// Make world matrix
		// (rotate and then translate the object)
		// (order matters)
		l3d_quatToRotMat(&mat_rot, &cam->orientation);
		l3d_mat4x4_makeTranslation(&mat_trans, &cam->local_pos);
		l3d_mat4x4_mulMatrix(&mat_world, &mat_trans, &mat_rot);

		l3d_transformObjectIntoWorldSpace(scene, L3D_OBJ_TYPE_CAMERA, cam_id, &mat_world);
	}
	
	// Obj3d
	for (uint16_t obj_id = 0; obj_id < scene->object_count; obj_id++) {
		obj3d = &(scene->objects[obj_id]);
		if (obj3d == NULL)
			return L3D_DATA_EMPTY;
		
		// Make world matrix
		// (rotate and then translate the object)
		// (order matters)
		l3d_quatToRotMat(&mat_rot, &obj3d->orientation);
		l3d_mat4x4_makeTranslation(&mat_trans, &obj3d->local_pos);
		l3d_mat4x4_mulMatrix(&mat_world, &mat_trans, &mat_rot);

		l3d_transformObjectIntoWorldSpace(scene, L3D_OBJ_TYPE_OBJ3D, obj_id, &mat_world);
	}

	return L3D_OK;
}

// 
// Draw wireframe of a signle 3D object
// 
l3d_err_t l3d_drawWireframe(const l3d_scene_t *scene, uint16_t obj_id) {
	l3d_obj3d_t *obj3d = &(scene->objects[obj_id]);
	if (obj3d == NULL)
		return L3D_DATA_EMPTY;

	uint16_t edge_data_offset = obj3d->mesh.model_edge_data_offset;// * 3;

	// L3D_DEBUG_PRINT("obj idx: %d, obj3d->mesh.model_edge_data_offset = %d, edge_data_offset = %d\n",
	// 	obj_id, obj3d->mesh.model_edge_data_offset, edge_data_offset);

	// This draws all the edges of all meshes
	// for (uint16_t edge_data_idx = edge_data_offset; edge_data_idx < edge_data_offset + scene->model_edge_count * 3; edge_data_idx += 3) {
	
	// For each edge of the object's mesh
	for (uint16_t edge_data_idx = edge_data_offset; edge_data_idx < edge_data_offset + obj3d->mesh.edge_count * 3; edge_data_idx += 3) {
		// If edge invisible: continue
		uint16_t edge_id = edge_data_idx/3;

		// L3D_DEBUG_PRINT("obj idx: %d: edge_data_idx = %d, edge_id = %d\n",
		// 	obj_id, edge_data_idx, edge_id);
		
		uint8_t flags = scene->edge_flags[edge_id];
		if (!L3D_IS_EDGE_VISISBLE(flags))
			continue;
		
		// set to zero when only multiple instances
		// set to transformed_vertices_offset when many meshes each with a single instance
		uint16_t tr_vert_offset = obj3d->mesh.transformed_vertices_offset;

		// Get projected vertices
		uint16_t v1_id = scene->model_edge_data[edge_data_idx+0] + tr_vert_offset;// + obj3d->mesh.model_vert_data_offset/3;
		uint16_t v2_id = scene->model_edge_data[edge_data_idx+1] + tr_vert_offset;// + obj3d->mesh.model_vert_data_offset/3;
		// uint16_t tri_id = scene->model_edge_data[edge_id+2];

		// L3D_DEBUG_PRINT("obj idx: %d: model_vert_data_offset = %d, obj3d->mesh.transformed_vertices_offset = %d\n",
		// 	obj_id, obj3d->mesh.model_vert_data_offset, obj3d->mesh.transformed_vertices_offset);

		// L3D_DEBUG_PRINT("obj idx: %d: vm1_id = %d, vm2_id = %d\n",
		// 	obj_id, v1_id, v2_id);
		
		// L3D_DEBUG_PRINT("obj idx: %d: vp1_id = %d, vp2_id = %d\n",
		// 	obj_id, v1_id, v2_id);

		l3d_vec4_t v1 = scene->vertices_projected[v1_id];
		l3d_vec4_t v2 = scene->vertices_projected[v2_id];

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
#ifdef L3D_DRAW_INNER_EDGES
		else
			l3d_drawLineCallback(
				l3d_rationalToInt32(v1.x), l3d_rationalToInt32(v1.y),
				l3d_rationalToInt32(v2.x), l3d_rationalToInt32(v2.y),
				L3D_DEBUG_VISIBLE_EDGE_COLOUR);
#endif	// L3D_DRAW_INNER_EDGES
#else
#ifdef L3D_DRAW_INNER_EDGES
	// Draw all edges
	l3d_drawLineCallback(
		l3d_rationalToInt32(v1.x), l3d_rationalToInt32(v1.y),
		l3d_rationalToInt32(v2.x), l3d_rationalToInt32(v2.y),
		obj3d->wireframe_colour);
#else
	// Draw only boundary edges
	if (L3D_IS_EDGE_BOUNDARY(flags)) {
		l3d_drawLineCallback(
			l3d_rationalToInt32(v1.x), l3d_rationalToInt32(v1.y),
			l3d_rationalToInt32(v2.x), l3d_rationalToInt32(v2.y),
			obj3d->wireframe_colour);
	}
#endif	// L3D_DRAW_INNER_EDGES
#endif	// L3D_DEBUG_EDGES
	}

	return L3D_OK;
}

// 
// Draw gizmos of given object.
// For now, only its location marker.
// 
l3d_err_t l3d_drawGizmos(const l3d_scene_t *scene, uint16_t obj_id) {
	l3d_obj3d_t *obj3d = &(scene->objects[obj_id]);
	if (obj3d == NULL)
		return L3D_DATA_EMPTY;

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
		L3D_COLOUR_DARKRED);
	// Y
	l3d_drawLineCallback(
		l3d_rationalToInt32(global_axes_proj[2].x), l3d_rationalToInt32(global_axes_proj[2].y),
		l3d_rationalToInt32(global_axes_proj[0].x), l3d_rationalToInt32(global_axes_proj[0].y),
		L3D_COLOUR_DARKGREEN);
	// Z
	l3d_drawLineCallback(
		l3d_rationalToInt32(global_axes_proj[3].x), l3d_rationalToInt32(global_axes_proj[3].y),
		l3d_rationalToInt32(global_axes_proj[0].x), l3d_rationalToInt32(global_axes_proj[0].y),
		L3D_COLOUR_DARKBLUE);
	
	return L3D_OK;
}

// 
// Draw every object in given scene
// 
l3d_err_t l3d_drawObjects(const l3d_scene_t *scene) {
	if (scene == NULL ||
		scene->objects == NULL ||
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
// Compute projection and view matrices if needed,
// transform objects into view space,
// and draw them
// 
l3d_err_t l3d_processScene(l3d_scene_t *scene) {
	if (scene == NULL)
		return L3D_WRONG_PARAM;

	l3d_camera_t *cam = l3d_scene_getActiveCamera(scene);
	// cam can not be NULL, because scene is not NULL

	// Update projection matrix if needed
	if (cam->is_modified) {
		l3d_makeProjectionMatrix(&(scene->mat_proj), cam);
		cam->is_modified = false;
	}

#ifdef L3D_CAMERA_MOVABLE
	// Update view matrix if needed
	if (cam->has_moved) {
		l3d_computeViewMatrix(cam, &(scene->mat_view));
		cam->has_moved = false;
	}
#endif
	
	l3d_err_t ret;
	// ret = l3d_processObjects(scene, &(scene->mat_proj), &(scene->mat_view));

	// if (ret != L3D_OK)
	// 	return ret;

	for (uint16_t obj_idx=0; obj_idx<scene->object_count; obj_idx++) {
		l3d_transformObjectIntoViewSpace(scene, L3D_OBJ_TYPE_OBJ3D, obj_idx);
	}

	ret = l3d_drawObjects(scene);

	return ret;
}

