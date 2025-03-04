#include "../Inc/lib3d_core.h"

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

    l3d_vec4_t v_forward = l3d_vec4_mul( &(cam->local_look_dir), l3d_floatToRational( 4.0f * f_elapsed_time ) );
	l3d_vec4_t v_up = l3d_getZeroVec4();
	v_up.z = l3d_floatToRational(1.0f);		// Z-axis points up
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
// For now simply draw all the objects
// 
l3d_err_t l3d_processScene(l3d_scene_t *scene, l3d_mat4x4_t *mat_proj, l3d_mat4x4_t *mat_view) {
	// l3d_err_t l3d_processScene(l3d_scene_t *scene) {
	if (scene == NULL)
		return L3D_WRONG_PARAM;
	
	// l3d_mat4x4_t mat_proj, mat_view;

	// l3d_makeProjectionMatrix(&mat_proj, scene->active_camera);
	// l3d_computeViewMatrix(scene->active_camera, &mat_view, );
	
	// l3d_err_t ret;
	// ret = l3d_processObjects(scene, &mat_proj, &mat_view);

	
	l3d_err_t ret;
	ret = l3d_processObjects(scene, mat_proj, mat_view);

	if (ret != L3D_OK)
		return ret;

	// Draw each object
	// - draw its edges
	ret = l3d_drawObjects(scene);

	return ret;
}

// 
// No backface culling yet,
// just to see if bare minimum program works
// 
l3d_err_t l3d_processObjects(l3d_scene_t *scene, const l3d_mat4x4_t *mat_proj, const l3d_mat4x4_t *mat_view) {
	if (scene == NULL)
		return L3D_WRONG_PARAM;
	// Process each object's vertices
	l3d_obj3d_t *obj3d = NULL;
	l3d_mat4x4_t mat_rot_x, mat_rot_y, mat_rot_z, mat_trans, mat_world, mat_tmp;
	
	for (uint16_t obj_id = 0; obj_id < scene->object_count; obj_id++) {
		obj3d = &(scene->objects[obj_id]);
		if (obj3d == NULL)
			return L3D_DATA_EMPTY;
		
		// Rotate the object
		l3d_mat4x4_makeRotX(&mat_rot_x, obj3d->local_rot.pitch);
		l3d_mat4x4_makeRotY(&mat_rot_y, obj3d->local_rot.roll);
		l3d_mat4x4_makeRotZ(&mat_rot_z, obj3d->local_rot.yaw);
		// Translate the object
		l3d_mat4x4_makeTranslation(&mat_trans, obj3d->local_pos.x, obj3d->local_pos.y, obj3d->local_pos.z);
		// Make world matrix
		l3d_mat4x4_makeIdentity(&mat_world);
		l3d_mat4x4_makeIdentity(&mat_tmp);
		l3d_mat4x4_mulMatrix(&mat_tmp, &mat_rot_z, &mat_rot_x);
		l3d_mat4x4_mulMatrix(&mat_world, &mat_tmp, &mat_trans);
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

			scene->vertices_world[tr_vert_offset + v_id] = vertex; // shallow copy is sufficient
		}

		// Transform all vertices to view space
		// and project them onto 2D screen coordinates
		for (uint16_t v_id = 0; v_id < vert_count; v_id++ ) {
			l3d_vec4_t v_world = scene->vertices_world[tr_vert_offset + v_id];
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
			scene->vertices_projected[v_id] = v_projected;
		}
	}

	return L3D_OK;
}

l3d_err_t l3d_drawObjects(const l3d_scene_t *scene) {
	if (scene == NULL)
		return L3D_WRONG_PARAM;

	l3d_obj3d_t *obj3d = NULL;
	for (uint16_t obj_id = 0; obj_id < scene->object_count; obj_id++) {
		obj3d = &(scene->objects[obj_id]);
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

			// Draw the edge
			l3d_drawLineCallback(v1.x, v1.y, v2.x, v2.y, obj3d->wireframe_colour);
		}
	}

	// for (uint16_t edge_id = 0; edge_id < scene->model_edge_count * 3; edge_id += 3) {
	// 	// If edge invisible: continue
	// 	// Get projected vertices
	// 	uint16_t v1_id = scene->model_edge_data[edge_id+0];
	// 	uint16_t v2_id = scene->model_edge_data[edge_id+1];
	// 	// uint16_t tri_id = scene->model_edge_data[edge_id+2];

	// 	l3d_vec4_t v1 = scene->vertices_projected[v1_id];
	// 	l3d_vec4_t v2 = scene->vertices_projected[v2_id];
	// 	// Draw edge
	// 	l3d_drawLineCallback(v1.x, v1.y, v2.x, v2.y, object_wireframe_colour);
	// }

	return L3D_OK;
}