#include "lib3d_camera.h"

l3d_err_t l3d_cam_reset(l3d_camera_t *cam){
	cam->local_pos = l3d_getZeroVec4();
	cam->local_rot = l3d_getZeroRot();
	cam->local_look_dir = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);	// Y-axis points forward
	cam->local_up_dir = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);	// Z-axis points up
	cam->fov = l3d_floatToRational(90.0f);
	cam->near_plane = l3d_floatToRational(0.1f);
	cam->far_plane = l3d_floatToRational(1000.0f);
	cam->type = L3D_CAMERA_TYPE_FIRST_PERSON;

	return L3D_OK;
}