#include "../Inc/lib3d_camera.h"

// 
// Reset camera to its default values
// 
l3d_err_t l3d_cam_reset(l3d_camera_t *cam){
	if (cam == NULL)
		return L3D_WRONG_PARAM;
	
	cam->local_pos = l3d_getZeroVec4();
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
	// cam->type = L3D_CAMERA_TYPE_FIRST_PERSON;	// may be implemented in the future

	cam->has_moved = 0;
	cam->is_modified = 0;

	return L3D_OK;
}

l3d_err_t l3d_cam_getFov(l3d_camera_t *cam, l3d_rtnl_t *fov) {
	if (cam == NULL)
		return L3D_WRONG_PARAM;
	
	*fov = cam->fov;

	return L3D_OK;
}

l3d_err_t l3d_cam_setFov(l3d_camera_t *cam, l3d_rtnl_t fov) {
	if (cam == NULL ||
		fov <= L3D_EPSILON_RTNL ||
		fov >= l3d_floatToRational(180))
	{
		return L3D_WRONG_PARAM;
	}
		
	cam->fov = fov;
	cam->is_modified = true;
	return L3D_OK;
}

l3d_err_t l3d_cam_getNearPlane(l3d_camera_t *cam, l3d_rtnl_t *near_plane) {
	if (cam == NULL)
		return L3D_WRONG_PARAM;
	
	*near_plane = cam->near_plane;

	return L3D_OK;
}

l3d_err_t l3d_cam_setNearPlane(l3d_camera_t *cam, l3d_rtnl_t near_plane) {
	if (cam == NULL ||
		near_plane <= L3D_EPSILON_RTNL ||
		near_plane >= cam->far_plane)
	{
		return L3D_WRONG_PARAM;
	}

	cam->near_plane = near_plane;
	cam->is_modified = true;
	return L3D_OK;
}

l3d_err_t l3d_cam_getFarPlane(l3d_camera_t *cam, l3d_rtnl_t *far_plane) {
	if (cam == NULL)
		return L3D_WRONG_PARAM;
	
	*far_plane = cam->far_plane;

	return L3D_OK;
}

l3d_err_t l3d_cam_setFarPlane(l3d_camera_t *cam, l3d_rtnl_t far_plane) {
	if (cam == NULL ||
		far_plane <= cam->near_plane)
	{
		return L3D_WRONG_PARAM;
	}

	cam->far_plane = far_plane;
	cam->is_modified = true;
	return L3D_OK;
}