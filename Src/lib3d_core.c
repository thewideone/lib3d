#include "../Inc/lib3d_core.h"

// 
// Set up projection matrix for mesh transformation.
// mat	- matrix to be set up as a projection mat
// 
void l3d_makeProjectionMatrix(l3d_mat4x4_t *mat, l3d_camera_t *cam){
// #ifdef L3D_USE_FIXED_POINT_ARITHMETIC
	l3d_mat4x4_makeProjection(
			mat,
			cam->fov,
			l3d_floatToRational( (l3d_flp_t)SCREEN_HEIGHT / (l3d_flp_t)SCREEN_WIDTH ),
			cam->near_plane,
			cam->far_plane );
// #else
// 	l3d_mat4x4_makeProjection( mat,
// 			90.0,
// 			(l3d_flp_t)SCREEN_HEIGHT / (l3d_flp_t)SCREEN_WIDTH,
// 			0.1f,
// 			1000.0f );
// #endif
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

void l3d_processScene(l3d_scene_t *scene){

}