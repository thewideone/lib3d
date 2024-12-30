#include "lib3d_camera.h"

l3d_err_t l3d_cam_reset(l3d_camera_t *cam){
	cam->local_pos = l3d_getZeroVec();
	cam->local_rot = l3d_getZeroRot();


	return L3D_OK;
}