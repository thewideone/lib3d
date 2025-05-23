#include "../Inc/lib3d_scene.h"

// Used only by l3d_scene_getObjectLocalUnitVec...()
// May change in the future
typedef enum l3d_dummy_axis_enum {
    L3D_AXIS_LOCAL_POS, // duplicate of local_pos; remove this or local_pos
    L3D_AXIS_X,
    L3D_AXIS_Y,
    L3D_AXIS_Z
} l3d_axis_t;

// 
// Get active camera of given scene
// 
// TODO: maybe replace pointer with index
l3d_camera_t *l3d_scene_getActiveCamera(l3d_scene_t *scene){
	if (scene == NULL)
		return NULL;
	return &scene->cameras[scene->active_camera_idx];
}

// uint16_t l3d_scene_getActiveCameraIdx(l3d_scene_t *scene){
// 	return scene->active_camera_idx;
// }

// 
// Set active camera of given scene
// 
l3d_err_t l3d_scene_setActiveCamera(l3d_scene_t *scene, uint16_t cam_idx){
	if (scene == NULL || cam_idx >= scene->camera_count)
		return L3D_WRONG_PARAM;
	
	scene->active_camera_idx = &scene->cameras[cam_idx];
}

l3d_vec4_t l3d_scene_getObjectLocalPos(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	if (scene != NULL) {
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
	}
	return l3d_getZeroVec4();
}

l3d_err_t l3d_scene_setObjectLocalPos(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *pos) {
	if (scene == NULL || pos == NULL)
		return L3D_WRONG_PARAM;
	
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

l3d_quat_t l3d_scene_getObjectOrientation(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	if (scene != NULL) {
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
			default:
				break;
		}
	}
	return l3d_getIdentityQuat();
}

l3d_err_t l3d_scene_setObjectOrientation(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q) {
	if (scene == NULL || q == NULL)
		return L3D_WRONG_PARAM;
	
	l3d_obj3d_t *obj = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return L3D_DATA_EMPTY;
			cam->orientation = *q;
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			obj = &scene->objects[idx];
			if (obj == NULL)
				return L3D_DATA_EMPTY;
			obj->orientation = *q;
			break;
		default:
			return L3D_WRONG_PARAM;
	}
	return L3D_OK;
}

l3d_vec4_t l3d_scene_getObjectLocalUnitVecIdx(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_axis_t axis_idx) {
	if (scene != NULL) {
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
