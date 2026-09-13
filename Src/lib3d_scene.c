#include "../Inc/lib3d_scene.h"
#include "lib3d_text3d.h"
#include "lib3d_util.h"
#include "lib3d_transform.h"

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
l3d_camera_t *l3d_scene_getActiveCamera(const l3d_scene_t *scene){
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
	
	scene->active_camera_idx = cam_idx;
	return L3D_OK;
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
			case L3D_OBJ_TYPE_CHAR3D:
				if (scene->chars3d == NULL || idx >= scene->char3d_count)
					break;	// DATA_EMPTY or WRONG_PARAM
				return scene->chars3d[idx].obj3d.local_pos;
			// case L3D_OBJ_TYPE_STR3D:
			// 	if (scene->strs3d == NULL || idx >= scene->str3d_count)
			// 		break;	// DATA_EMPTY or WRONG_PARAM
			// 	return scene->strs3d[idx].local_pos;
		}
	}
	return l3d_getZeroVec4();
}

l3d_err_t l3d_scene_setObjectLocalPos(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *pos) {
	if (scene == NULL || pos == NULL)
		return L3D_WRONG_PARAM;
	
	l3d_obj3d_t *obj = NULL;
	l3d_camera_t *cam = NULL;
	l3d_char3d_t *char3d = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			if (scene->cameras == NULL)
				return L3D_DATA_EMPTY;
			if (idx >= scene->camera_count)
				return L3D_WRONG_PARAM;
				
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return L3D_DATA_EMPTY;
			cam->local_pos = *pos;
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			if (scene->objects == NULL)
				return L3D_DATA_EMPTY;
			if (idx >= scene->object_count)
				return L3D_WRONG_PARAM;

			obj = &scene->objects[idx];
			if (obj == NULL)
				return L3D_DATA_EMPTY;
			obj->local_pos = *pos;
			break;
		case L3D_OBJ_TYPE_CHAR3D:
			if (scene->chars3d == NULL)
				return L3D_DATA_EMPTY;
			if (idx >= scene->char3d_count)
				return L3D_WRONG_PARAM;

			char3d = &scene->chars3d[idx];
			if (char3d == NULL)
				return L3D_DATA_EMPTY;
			char3d->obj3d.local_pos = *pos;
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

l3d_err_t l3d_scene_setChar(l3d_scene_t *scene, const uint8_t char_idx, char c) {
	if (scene == NULL || scene->chars3d == NULL || char_idx >= scene->char3d_count)
		return L3D_WRONG_PARAM;

	l3d_char3d_t *char3d = &scene->chars3d[char_idx];
	if (char3d == NULL || char3d->font_desc == NULL)
		return L3D_DATA_EMPTY;

	l3d_err_t ret = l3d_char3d_setChar(char3d, c);

	if (ret != L3D_OK) {
		L3D_DEBUG_PRINT("l3d_char3d_setChar() failed (%d)\n", ret);
		return ret;
	}

	// From lib3d_core.c: l3d_setupObjects():
	l3d_mat4x4_t mat_rot, mat_trans, mat_world;
	l3d_quatToRotMat(&mat_rot, &char3d->obj3d.orientation);
	l3d_mat4x4_makeTranslation(&mat_trans, &char3d->obj3d.local_pos);
	l3d_mat4x4_mulMatrix(&mat_world, &mat_trans, &mat_rot);

	ret = l3d_transformObjectIntoWorldSpace(scene, L3D_OBJ_TYPE_CHAR3D, char_idx, &mat_world);

	if (ret != L3D_OK) {
		L3D_DEBUG_PRINT("l3d_transformObjectIntoWorldSpace() failed (%d)\n", ret);
		return ret;
	}

	// ret = l3d_transformObjectIntoViewSpace(scene, L3D_OBJ_TYPE_CHAR3D, char_idx);

	// if (ret != L3D_OK) {
	// 	L3D_DEBUG_PRINT("l3d_transformObjectIntoViewSpace() failed (%d)\n", ret);
	// 	return ret;
	// }

	return ret;
}

// // If success, return index of the char3d in scene->char_ptrs array
// l3d_err_t l3d_scene_linkChar3d(l3d_scene_t *scene, const l3d_char3d_t *char3d, uint8_t *idx) {
// 	if (scene == NULL || char3d == NULL)
// 		return L3D_WRONG_PARAM;
	
// 	for (uint16_t i=0; i < L3D_SCENE_MAX_CHAR3D_COUNT; i++) {
// 		if (scene->char_ptrs[i] == NULL) {
// 			// Store the pointer and save its index
// 			scene->char_ptrs[i] = char3d;
// 			*idx = i;
// 			return L3D_OK;
// 		}
// 	}

// 	return L3D_BUFF_OVF;
// }

// l3d_err_t l3d_scene_unlinkChar3d(l3d_scene_t *scene, const uint8_t idx) {
// 	if (scene == NULL || idx >= L3D_SCENE_MAX_CHAR3D_COUNT)
// 		return L3D_WRONG_PARAM;
	
// 	if (scene->char_ptrs[idx] != NULL) {
// 		scene->char_ptrs[idx] = NULL;
// 		return L3D_OK;
// 	}
	
// 	return L3D_DATA_EMPTY;
// }

