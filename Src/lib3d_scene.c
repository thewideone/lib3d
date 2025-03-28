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

l3d_vec4_t l3d_scene_getObjectLocalPos(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			l3d_camera_t *cam = &scene->cameras[idx];
			if (cam == NULL)
				break;
			return cam->local_pos;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj = &scene->objects[idx];
			if (obj == NULL)
				break;
			return obj->local_pos;
	}
	return l3d_getZeroVec4();
}

l3d_err_t l3d_scene_setObjectLocalPos(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *pos) {
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			l3d_camera_t *cam = &scene->cameras[idx];
			if (cam == NULL)
				return L3D_DATA_EMPTY;
			cam->local_pos = *pos;
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj = &scene->objects[idx];
			if (obj == NULL)
				return L3D_DATA_EMPTY;
			obj->local_pos = *pos;
			break;
		default:
			return L3D_WRONG_PARAM;
	}
	return L3D_OK;
}

l3d_rot_t l3d_scene_getObjectLocalRot(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			l3d_camera_t *cam = &scene->cameras[idx];
			if (cam == NULL)
				break;
			return cam->local_rot;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj = &scene->objects[idx];
			if (obj == NULL)
				break;
			return obj->local_rot;
	}
	return l3d_getZeroRot();
}

l3d_err_t l3d_scene_setObjectLocalRot(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_rot_t *rot) {
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			l3d_camera_t *cam = &scene->cameras[idx];
			if (cam == NULL)
				return L3D_DATA_EMPTY;
			cam->local_rot = *rot;
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj = &scene->objects[idx];
			if (obj == NULL)
				return L3D_DATA_EMPTY;
			obj->local_rot = *rot;
			break;
		default:
			return L3D_WRONG_PARAM;
	}
	return L3D_OK;
}