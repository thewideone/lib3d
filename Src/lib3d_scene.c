#include "../Inc/lib3d_scene.h"

// Redundant?
// Or replace pointer with index
l3d_obj3d_t *l3d_scene_getObjectByIdx(l3d_scene_t *scene, uint16_t idx){
	if (idx >= scene->object_count)
		return NULL;
	
	return &(scene->objects[idx]);
}

// Replace pointer with index
l3d_camera_t *l3d_scene_getActiveCamera(l3d_scene_t *scene){
	return scene->active_camera;
}

// Replace pointer with index
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
	l3d_obj3d_t *obj = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				break;
			return cam->local_pos;
		case L3D_OBJ_TYPE_OBJ3D:
			obj = &scene->objects[idx];
			if (obj == NULL)
				break;
			return obj->local_pos;
	}
	return l3d_getZeroVec4();
}

l3d_err_t l3d_scene_setObjectLocalPos(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *pos) {
	l3d_obj3d_t *obj = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return L3D_DATA_EMPTY;
			cam->local_pos = *pos;
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			obj = &scene->objects[idx];
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
	l3d_obj3d_t *obj = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				break;
			return cam->local_rot;
		case L3D_OBJ_TYPE_OBJ3D:
			obj = &scene->objects[idx];
			if (obj == NULL)
				break;
			return obj->local_rot;
	}
	return l3d_getZeroRot();
}

l3d_err_t l3d_scene_setObjectLocalRot(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_rot_t *rot) {
	l3d_camera_t *cam = NULL;
	l3d_obj3d_t *obj = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return L3D_DATA_EMPTY;
			cam->local_rot = *rot;
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			obj = &scene->objects[idx];
			if (obj == NULL)
				return L3D_DATA_EMPTY;
			obj->local_rot = *rot;
			break;
		default:
			return L3D_WRONG_PARAM;
	}
	return L3D_OK;
}

l3d_quat_t l3d_scene_getObjectOrientation(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	l3d_obj3d_t *obj = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				break;
			return cam->orientation;
		case L3D_OBJ_TYPE_OBJ3D:
			obj = &scene->objects[idx];
			if (obj == NULL)
				break;
			return obj->orientation;
	}
	return l3d_getIdentityQuat();
}

l3d_quat_t l3d_scene_setObjectOrientation(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q) {
	l3d_obj3d_t *obj = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				break;
			cam->orientation = *q;
		case L3D_OBJ_TYPE_OBJ3D:
			obj = &scene->objects[idx];
			if (obj == NULL)
				break;
			obj->orientation = *q;
	}
	return l3d_getIdentityQuat();
}

l3d_vec4_t l3d_scene_getObjectLocalUnitVecIdx(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_axis_t axis_idx) {
	l3d_camera_t *cam = NULL;
	l3d_obj3d_t *obj = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				break;
			return l3d_vec4_sub(&cam->u_world[axis_idx], &cam->u_world[L3D_AXIS_LOCAL_POS]);	// replace u_world[] with local_pos
		case L3D_OBJ_TYPE_OBJ3D:
			obj = &scene->objects[idx];
			if (obj == NULL)
				break;
			return l3d_vec4_sub(&obj->u_world[axis_idx], &obj->u_world[L3D_AXIS_LOCAL_POS]);	// replace u_world[] with local_pos
	}
	return l3d_getZeroVec4();
}

l3d_vec4_t l3d_scene_getObjectLocalUnitVecX(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	return l3d_scene_getObjectLocalUnitVecIdx(scene, type, idx, L3D_AXIS_X);
}

l3d_vec4_t l3d_scene_getObjectLocalUnitVecY(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	return l3d_scene_getObjectLocalUnitVecIdx(scene, type, idx, L3D_AXIS_Y);
}

l3d_vec4_t l3d_scene_getObjectLocalUnitVecZ(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	return l3d_scene_getObjectLocalUnitVecIdx(scene, type, idx, L3D_AXIS_Z);
}
