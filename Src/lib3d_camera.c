#include "../Inc/lib3d_camera.h"

l3d_err_t l3d_cam_reset(l3d_camera_t *cam){
	cam->local_pos = l3d_getZeroVec4();
	cam->local_rot = l3d_getZeroRot();	// to be removed
	cam->orientation = l3d_getIdentityQuat();
	cam->u[0] = l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);
	cam->u[1] = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
	cam->u[2] = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
	cam->u[3] = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
	cam->local_look_dir = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);	// Y-axis points forward
	cam->local_up_dir = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);	// Z-axis points up
	cam->fov = l3d_floatToRational(L3D_CAMERA_DEFAULT_FOV);
	cam->near_plane = l3d_floatToRational(L3D_CAMERA_DEFAULT_NEAR_PLANE);
	cam->far_plane = l3d_floatToRational(L3D_CAMERA_DEFAULT_FAR_PLANE);
	cam->type = L3D_CAMERA_TYPE_FIRST_PERSON;

	return L3D_OK;
}