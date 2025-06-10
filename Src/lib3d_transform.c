#include "../Inc/lib3d_transform.h"
#include "../Inc/lib3d_util.h"	// for debug print
#include <assert.h>	// for debug

// 
// This function applies given transformation matrix to given object in world space
// 
l3d_err_t l3d_applyTransformMatrix(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_mat4x4_t *mat_transform) {
	if (scene == NULL || mat_transform == NULL)
		return L3D_DATA_EMPTY;
	
	l3d_obj3d_t *obj3d = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			if (idx > scene->camera_count)
				return L3D_WRONG_PARAM;
			
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return L3D_DATA_EMPTY;
			
			// Transform local position
			cam->local_pos = l3d_mat4x4_mulVec4(mat_transform, &cam->local_pos);

			// Transform orientation markers
			cam->u_world[0] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[0]);
			cam->u_world[1] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[1]);
			cam->u_world[2] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[2]);
			cam->u_world[3] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[3]);

			cam->has_moved = true;

			// for each child: transform it... really here or in the caller function?
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			if (idx > scene->object_count)
				return L3D_WRONG_PARAM;
			
			obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return L3D_DATA_EMPTY;
			
			// Transform local position
			obj3d->local_pos = l3d_mat4x4_mulVec4(mat_transform, &obj3d->local_pos);

			// Transform all vertices
			uint16_t vert_count = obj3d->mesh.vert_count;
			// uint16_t model_vert_data_offset = obj3d->mesh.model_vert_data_offset;
			uint16_t tr_vert_offset = obj3d->mesh.transformed_vertices_offset;

			for (uint16_t v_id = 0; v_id < vert_count; v_id++) {
				// Get vertex from vertex data of current object's mesh
				l3d_vec4_t vertex = scene->vertices_world[tr_vert_offset + v_id];

				l3d_vec4_t v_transformed = l3d_mat4x4_mulVec4(mat_transform, &vertex);

				scene->vertices_world[tr_vert_offset + v_id] = v_transformed; // shallow copy is sufficient
			}

			// Transform orientation markers
			obj3d->u_world[0] = l3d_mat4x4_mulVec4(mat_transform, &obj3d->u_world[0]);
			obj3d->u_world[1] = l3d_mat4x4_mulVec4(mat_transform, &obj3d->u_world[1]);
			obj3d->u_world[2] = l3d_mat4x4_mulVec4(mat_transform, &obj3d->u_world[2]);
			obj3d->u_world[3] = l3d_mat4x4_mulVec4(mat_transform, &obj3d->u_world[3]);

			obj3d->updated = true;

			// for each child: transform it... really here or in the caller function?
			break;
		default:
			return L3D_WRONG_PARAM;
	}

	return L3D_OK;
}

// 
// Additive translation
// Instead of multiplying by a matrix, add delta_pos to each vertex position
// 
l3d_err_t l3d_additiveTranslateObject(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
	if (scene == NULL || delta_pos == NULL)
		return L3D_DATA_EMPTY;

	l3d_obj3d_t *obj3d = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			if (idx > scene->camera_count)
				return L3D_WRONG_PARAM;
			
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return L3D_DATA_EMPTY;
			
			// Translate local position
			cam->local_pos = l3d_vec4_add(delta_pos, &cam->local_pos);

			// Translate orientation markers
			cam->u_world[0] = l3d_vec4_add(delta_pos, &cam->u_world[0]);
			cam->u_world[1] = l3d_vec4_add(delta_pos, &cam->u_world[1]);
			cam->u_world[2] = l3d_vec4_add(delta_pos, &cam->u_world[2]);
			cam->u_world[3] = l3d_vec4_add(delta_pos, &cam->u_world[3]);

			cam->has_moved = true;

			// for each child: Translate it... really here or in the caller function?
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			if (idx > scene->object_count)
				return L3D_WRONG_PARAM;
			
			obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return L3D_DATA_EMPTY;
			
			obj3d->local_pos = l3d_vec4_add(delta_pos, &obj3d->local_pos);
			// Translate all vertices
			uint16_t vert_count = obj3d->mesh.vert_count;
			// uint16_t model_vert_data_offset = obj3d->mesh.model_vert_data_offset;
			uint16_t tr_vert_offset = obj3d->mesh.transformed_vertices_offset;

			for (uint16_t v_id = 0; v_id < vert_count; v_id++) {
				// Get vertex from vertex data of current object's mesh
				l3d_vec4_t vertex = scene->vertices_world[tr_vert_offset + v_id];

				l3d_vec4_t v_transformed = l3d_vec4_add(delta_pos, &vertex);

				scene->vertices_world[tr_vert_offset + v_id] = v_transformed; // shallow copy is sufficient
			}

			// Translate orientation markers
			obj3d->u_world[0] = l3d_vec4_add(delta_pos, &obj3d->u_world[0]);
			obj3d->u_world[1] = l3d_vec4_add(delta_pos, &obj3d->u_world[1]);
			obj3d->u_world[2] = l3d_vec4_add(delta_pos, &obj3d->u_world[2]);
			obj3d->u_world[3] = l3d_vec4_add(delta_pos, &obj3d->u_world[3]);

			obj3d->updated = true;

			// for each child: translate it... really here or in the caller function?
			break;
		default:
			return L3D_WRONG_PARAM;
	}

	return L3D_OK;
}

// 
// Rotate object about the origin of the coordinate system
// using quaternion as input rotation description
// 
l3d_err_t l3d_rotateGlobalAboutOriginQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta) {
	// Update object's orientation
	// l3d_quat_t q_delta = l3d_axisAngleToQuat(axis, delta_angle_rad);
	l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	orientation = l3d_quat_mul(&orientation, q_delta);
	orientation = l3d_quat_normalise(&orientation);
	l3d_err_t ret = l3d_scene_setObjectOrientation(scene, type, idx, &orientation);
	if (ret != L3D_OK)
		return ret;

	// Transform the object
	l3d_mat4x4_t mat_rot;
	l3d_quatToRotMat(&mat_rot, q_delta);
	return l3d_applyTransformMatrix(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?
}

// 
// Rotate object about a pivot point
// using quaternion as input rotation description
// 
l3d_err_t l3d_rotateGlobalAboutPivotQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *pivot, const l3d_quat_t *q_delta) {
	// Move the object to the origin (0, 0, 0)
	l3d_vec4_t displacement = *pivot;
	displacement = l3d_vec4_negate(&displacement);
	l3d_err_t ret = l3d_additiveTranslateObject(scene, type, idx, &displacement);
	if (ret != L3D_OK)
		return ret;

	// Rotate the object about the origin
	ret = l3d_rotateGlobalAboutOriginQuat(scene, type, idx, q_delta);
	if (ret != L3D_OK)
		return ret;

	// Move the object to its initial position
	displacement = *pivot;
	return l3d_additiveTranslateObject(scene, type, idx, &displacement);
}

// 
// Rotate object at its position being the pivot point
// using quaternion given in global coordinates
// as input rotation description
// 
l3d_err_t l3d_rotateGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta) {
	l3d_vec4_t displacement = l3d_scene_getObjectLocalPos(scene, type, idx);
	return l3d_rotateGlobalAboutPivotQuat(scene, type, idx, &displacement, q_delta);
}


// 
// Rotate object at its position being the pivot point
// using quaternion given in object coordinates
// as input rotation description
// 
// l3d_err_t l3d_rotateLocalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta) {
// 	// q_final_global = q_delta * q_current_global
// 	// l3d_quat_t current_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
// 	// l3d_quat_t q_delta_global = l3d_quat_mul(q_delta, &current_orientation);
// 	// q_delta_global = l3d_quat_inverse(&q_delta_global);

// 	// q_delta_global = q_delta_local * inv(q_current)
// 	l3d_quat_t current_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
// 	current_orientation = l3d_quat_inverse(&current_orientation);
// 	l3d_quat_t q_delta_global = l3d_quat_mul(&current_orientation, q_delta);
// 	L3D_DEBUG_PRINT_QUAT_P(q_delta);
// 	L3D_DEBUG_PRINT_QUAT(current_orientation);
// 	L3D_DEBUG_PRINT_QUAT(q_delta_global);
	
// 	return l3d_rotateGlobalQuat(scene, type, idx, &q_delta_global);
// }

// 
// Rotate object about the origin of the coordinate system
// using axis-angle given in global coordinates
// as input rotation description
// 
l3d_err_t l3d_rotateGlobalAboutOriginAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad) {
	if (scene == NULL || axis == NULL)
		return L3D_DATA_EMPTY;
	
	// Update object's orientation
	l3d_quat_t q_delta = l3d_axisAngleToQuat(axis, delta_angle_rad);
	// l3d_rotateAboutOriginQuat(scene, type, idx, &q_delta);
	l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	orientation = l3d_quat_mul(&orientation, &q_delta);
	l3d_err_t ret = l3d_scene_setObjectOrientation(scene, type, idx, &orientation);
	if (ret != L3D_OK)
		return ret;

	// Transform the object
	l3d_mat4x4_t mat_rot;
	l3d_mat4x4_makeRot(&mat_rot, axis, delta_angle_rad);
	return l3d_applyTransformMatrix(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?
}

// 
// Rotate object about given pivot
// using axis-angle given in global coordinates
// as input rotation description
// 
l3d_err_t l3d_rotateGlobalAboutPivotAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *pivot, const l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad) {
	if (scene == NULL || pivot == NULL || axis == NULL)
		return L3D_DATA_EMPTY;
	
	// Move the object to the origin (0, 0, 0)
	l3d_vec4_t displacement = *pivot;
	displacement = l3d_vec4_negate(&displacement);
	l3d_err_t ret = l3d_additiveTranslateObject(scene, type, idx, &displacement);
	if (ret != L3D_OK)
		return ret;
	
	// Update object's orientation
	l3d_quat_t q_delta = l3d_axisAngleToQuat(axis, delta_angle_rad);
	l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	orientation = l3d_quat_mul(&orientation, &q_delta);
	orientation = l3d_quat_normalise(&orientation);
	ret = l3d_scene_setObjectOrientation(scene, type, idx, &orientation);
	if (ret != L3D_OK)
		return ret;

	// Rotate the object
	// Transform matrix could be made out of the quaternion q_delta aswell
	// It doesn't seem to be any difference
	l3d_mat4x4_t mat_transform;
	l3d_mat4x4_makeRot(&mat_transform, axis, delta_angle_rad);
	ret = l3d_applyTransformMatrix(scene, type, idx, &mat_transform);
	if (ret != L3D_OK)
		return ret;

	// Move the object to its initial position
	displacement = *pivot;
	return l3d_additiveTranslateObject(scene, type, idx, &displacement);
}

// 
// Rotate object about axis given in global space
// using axis-angle as input rotation description
// 
l3d_err_t l3d_rotateGlobalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t displacement = l3d_scene_getObjectLocalPos(scene, type, idx);
	return l3d_rotateGlobalAboutPivotAxisAngle(scene, type, idx, &displacement, axis, delta_angle_rad);
}

// 
// Rotate object about axis given in object space
// using axis-angle as input rotation description
// 
l3d_err_t l3d_rotateLocalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *local_axis, l3d_rtnl_t delta_angle_rad) {
	// Convert the axis given in object space to global space
	l3d_quat_t q_current = l3d_scene_getObjectOrientation(scene, type, idx);
	q_current = l3d_quat_normalise(&q_current);
	l3d_vec4_t global_axis = l3d_rotateVecByQuat(local_axis, &q_current);
	global_axis = l3d_vec4_normalise(&global_axis);

	// Object could be also transformed by axis-angle to quaternion aswell
	// l3d_quat_t q = l3d_axisAngleToQuat(&global_axis, delta_angle_rad);
	// q = l3d_quat_normalise(&q);
	// return l3d_rotateGlobalQuat(scene, type, idx, &q);

	return l3d_rotateGlobalAxisAngle(scene, type, idx, &global_axis, delta_angle_rad);
}

// 
// Rotate object about global X axis
// about angle in radians
// 
l3d_err_t l3d_rotateGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
	// l3d_quat_t q = l3d_axisAngleToQuat(&axis, delta_angle_rad);
	// q = l3d_quat_normalise(&q);
	// return l3d_rotateGlobalQuat(scene, type, idx, &q);
	return l3d_rotateGlobalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Rotate object about global Y axis
// about angle in radians
// 
l3d_err_t l3d_rotateGlobalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
	// l3d_quat_t q = l3d_axisAngleToQuat(&axis, delta_angle_rad);
	// q = l3d_quat_normalise(&q);
	// return l3d_rotateGlobalQuat(scene, type, idx, &q);
	return l3d_rotateGlobalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Rotate object about global Z axis
// about angle in radians
// 
l3d_err_t l3d_rotateGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
	// l3d_quat_t q = l3d_axisAngleToQuat(&axis, delta_angle_rad);
	// q = l3d_quat_normalise(&q);
	// return l3d_rotateGlobalQuat(scene, type, idx, &q);
	return l3d_rotateGlobalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Rotate object about its local X axis
// about angle in radians
// 
l3d_err_t l3d_rotateLocalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
	return l3d_rotateLocalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Rotate object about its local Y axis
// about angle in radians
// 
l3d_err_t l3d_rotateLocalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
	return l3d_rotateLocalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Rotate object about its local Z axis
// about angle in radians
// 
l3d_err_t l3d_rotateLocalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
	return l3d_rotateLocalAxisAngle(scene, type, idx, &axis, delta_angle_rad);
}

// 
// Reset object's orientation (in global coordinates)
// 
l3d_err_t l3d_resetOrientationGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	l3d_quat_t diff = l3d_scene_getObjectOrientation(scene, type, idx);
	diff = l3d_quat_inverse(&diff);
	diff = l3d_quat_normalise(&diff);

	return l3d_rotateGlobalQuat(scene, type, idx, &diff);

	// l3d_quat_t old_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	// l3d_quat_t product = l3d_quat_mul(&diff, &old_orientation);
	// product = l3d_quat_normalise(&product);
	// L3D_DEBUG_PRINT_QUAT(product);

	// assert(product.w - q_new->w < L3D_EPSILON_RTNL && 
	// 	product.x - q_new->x < L3D_EPSILON_RTNL && 
	// 	product.y - q_new->y < L3D_EPSILON_RTNL && 
	// 	product.z - q_new->z < L3D_EPSILON_RTNL);
	
	// l3d_mat4x4_t mat_rot;
	// l3d_quatToRotMat(&mat_rot, &diff);
	// l3d_applyTransformMatrix(scene, type, idx, &mat_rot);

	// // Update object's orientation
	// l3d_quat_t qi = l3d_getIdentityQuat();
	// l3d_scene_setObjectOrientation(scene, type, idx, &qi);
	// return L3D_OK;
}

// 
// Set object's orientation
// to a quaternion given in global coordinates
// 
l3d_err_t l3d_setOrientationGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_new) {
	// Reset object's orientation
	l3d_err_t ret = l3d_resetOrientationGlobal(scene, type, idx);
	if (ret != L3D_OK)
		return ret;
	return l3d_rotateGlobalQuat(scene, type, idx, q_new);
}

// Results in some strange results when combining quaternions
// l3d_err_t l3d_setOrientationGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_new) {
// 	// Transform the object
// 	// Compute multiplicative inverse
// 	// Equation taken from:
// 	// https://stackoverflow.com/a/22167097
// 	// diff * old_orientation == q_new (new_orientation)
// 	// diff = q_new * inverse(old_orientation)
// 	l3d_quat_t old_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
// 	L3D_DEBUG_PRINT_QUAT(old_orientation);
// 	// L3D_DEBUG_PRINT("old_rot norm = %.3f\n", l3d_rationalToFloat(l3d_quat_norm(&old_orientation)));
// 	old_orientation = l3d_quat_inverse(&old_orientation);
// 	// L3D_DEBUG_PRINT_QUAT(old_orientation);
// 	old_orientation = l3d_quat_normalise(&old_orientation);
// 	// L3D_DEBUG_PRINT_QUAT(old_orientation);
// 	l3d_quat_t diff = l3d_quat_mul(q_new, &old_orientation);
// 	// L3D_DEBUG_PRINT_QUAT(diff);
// 	diff = l3d_quat_normalise(&diff);
// 	L3D_DEBUG_PRINT_QUAT(diff);
	
// 	L3D_DEBUG_PRINT_QUAT_P(q_new);

// 	old_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
// 	l3d_quat_t product = l3d_quat_mul(&diff, &old_orientation);
// 	product = l3d_quat_normalise(&product);
// 	L3D_DEBUG_PRINT_QUAT(product);

// 	assert(product.w - q_new->w < L3D_EPSILON_RTNL && 
// 		product.x - q_new->x < L3D_EPSILON_RTNL && 
// 		product.y - q_new->y < L3D_EPSILON_RTNL && 
// 		product.z - q_new->z < L3D_EPSILON_RTNL);
	
// 	l3d_mat4x4_t mat_rot;
// 	l3d_quatToRotMat(&mat_rot, &diff);

// 	l3d_applyTransformMatrix(scene, type, idx, &mat_rot);

// 	// Update object's orientation
// 	l3d_scene_setObjectOrientation(scene, type, idx, q_new);
// 	return L3D_OK;
// }

// 
// Set object's orientation to orientation
// given by axis-angle given in global coordinates
// 
l3d_err_t l3d_setOrientationGlobalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t angle_rad) {
	// l3d_quat_t q = l3d_axisAngleToQuat(axis, angle_rad);
	// q = l3d_quat_normalise(&q);
	// return l3d_setOrientationGlobalQuat(scene, type, idx, &q);

	// Seems to be simpler this way - need to test it 
	l3d_err_t ret = l3d_resetOrientationGlobal(scene, type, idx);
	if (ret != L3D_OK)
		return ret;
	return l3d_rotateGlobalAxisAngle(scene, type, idx, axis, angle_rad);
}

// 
// Set object's orientation
// to Euler angles given in global space
// 
l3d_err_t l3d_setOrientationGlobalEuler(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_rot_t *r) {
	l3d_quat_t q = l3d_eulerToQuat(r);
	q = l3d_quat_normalise(&q);
	return l3d_setOrientationGlobalQuat(scene, type, idx, &q);
}

// 
// Move object by a vector given in global coordinates
// 
l3d_err_t l3d_moveGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
	return l3d_additiveTranslateObject(scene, type, idx, delta_pos);
}

// 
// Move object in global X axis by given distance
// 
l3d_err_t l3d_moveGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.x = delta_x;
	return l3d_additiveTranslateObject(scene, type, idx, &delta_pos);
}

// 
// Move object in global Y axis by given distance
// 
l3d_err_t l3d_moveGlobalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_y) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.y = delta_y;
	return l3d_additiveTranslateObject(scene, type, idx, &delta_pos);
}

// 
// Move object in global Z axis by given distance
// 
l3d_err_t l3d_moveGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_z) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.z = delta_z;
	return l3d_additiveTranslateObject(scene, type, idx, &delta_pos);
}

// 
// Move object in its local coordinates
// by a vector given in its local coordinates
// 
l3d_err_t l3d_moveLocal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
	// Get local direction unit vectors
	l3d_vec4_t v_displ_x = l3d_scene_getObjectLocalUnitVecX(scene, type, idx);
	l3d_vec4_t v_displ_y = l3d_scene_getObjectLocalUnitVecY(scene, type, idx);
	l3d_vec4_t v_displ_z = l3d_scene_getObjectLocalUnitVecZ(scene, type, idx);

	// Scale them by corresponding components of the position delta
	v_displ_x = l3d_vec4_mul(&v_displ_x, delta_pos->x);
	v_displ_y = l3d_vec4_mul(&v_displ_y, delta_pos->y);
	v_displ_z = l3d_vec4_mul(&v_displ_z, delta_pos->z);

	// Combine (add) displacement vectors
	l3d_vec4_t v_displacement = v_displ_x;
	v_displacement = l3d_vec4_add(&v_displacement, &v_displ_y);
	v_displacement = l3d_vec4_add(&v_displacement, &v_displ_z);

	return l3d_additiveTranslateObject(scene, type, idx, &v_displacement);
}

// 
// Move object in its local X axis by given distance
// 
l3d_err_t l3d_moveLocalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.x = delta_x;
	return l3d_moveLocal(scene, type, idx, &delta_pos);
}

// 
// Move object in its local Y axis by given distance
// 
l3d_err_t l3d_moveLocalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_y) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.y = delta_y;
	return l3d_moveLocal(scene, type, idx, &delta_pos);
}

// 
// Move object in its local Z axis by given distance
// 
l3d_err_t l3d_moveLocalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_z) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.z = delta_z;
	return l3d_moveLocal(scene, type, idx, &delta_pos);
}

// 
// Set object's global position
// 
l3d_err_t l3d_setGlobalPos(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *dest) {
	l3d_vec4_t current_pos = l3d_scene_getObjectLocalPos(scene, type, idx);
	l3d_vec4_t displacement = l3d_vec4_sub(dest, &current_pos);

	return l3d_additiveTranslateObject(scene, type, idx, &displacement);
}