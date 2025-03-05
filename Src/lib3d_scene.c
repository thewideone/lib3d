#include "../Inc/lib3d_scene.h"

l3d_obj3d_t *l3d_scene_getObjectByIdx(l3d_scene_t *scene, uint16_t idx){
	if (idx >= scene->object_count)
		return NULL;
	
	return &(scene->objects[idx]);
}

l3d_camera_t *l3d_scene_getActiveCamera(l3d_scene_t *scene){
	return scene->active_camera;
}

l3d_err_t l3d_scene_setActiveCamera(l3d_scene_t *scene, l3d_camera_t *cam){
	if (scene == NULL || cam == NULL)
		return L3D_WRONG_PARAM;
	
	// Check if cam is in scene->cameras
	// Only cam that belongs to scene can be set to active
	for (uint16_t i=0; i<scene->camera_count; i++)
		if (&scene->cameras[i] == cam) {
			scene->active_camera = cam;
			return L3D_OK;
		}
	
	return L3D_WRONG_PARAM;
}