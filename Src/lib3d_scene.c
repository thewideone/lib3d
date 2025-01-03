#include "lib3d_scene.h"

l3d_camera_t *l3d_scene_getActiveCamera(l3d_scene_t *scene){
	return scene->active_camera;
}

l3d_err_t l3d_scene_setActiveCamera(l3d_scene_t *scene, l3d_camera_t *cam){
	if (scene == NULL || cam == NULL)
		return L3D_WRONG_PARAM;
	
	// check if cam is in scene->cameras
	
	scene->active_camera = cam;

	return L3D_OK;
}