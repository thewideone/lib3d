#include "../Inc/lib3d_transform.h"
#include "../Inc/lib3d_util.h"	// for debug print
#include <assert.h>	// for debug

// 
// This function applies given transformation matrix to given object in world space
// 
void l3d_applyTransformMatrix(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_mat4x4_t *mat_transform) {
	l3d_obj3d_t *obj3d = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return;
			
			// Transform local position
			cam->local_pos = l3d_mat4x4_mulVec4(mat_transform, &cam->local_pos);

			// Transform orientation markers
			cam->u_world[0] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[0]);
			cam->u_world[1] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[1]);
			cam->u_world[2] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[2]);
			cam->u_world[3] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[3]);

			cam->updated = true;

			// for each child: transform it... really here or in the caller function?
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return;
			
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
	}
}

void l3d_translateObject(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
	l3d_obj3d_t *obj3d = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];
			if (cam == NULL)
				return;
			
			// Translate local position
			cam->local_pos = l3d_vec4_add(delta_pos, &cam->local_pos);

			// Translate orientation markers
			cam->u_world[0] = l3d_vec4_add(delta_pos, &cam->u_world[0]);
			cam->u_world[1] = l3d_vec4_add(delta_pos, &cam->u_world[1]);
			cam->u_world[2] = l3d_vec4_add(delta_pos, &cam->u_world[2]);
			cam->u_world[3] = l3d_vec4_add(delta_pos, &cam->u_world[3]);

			cam->updated = true;

			// for each child: Translate it... really here or in the caller function?
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return;
			
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
	}
}

l3d_err_t l3d_rotateAboutOriginQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta) {
	// Update object's orientation
	// l3d_quat_t q_delta = l3d_axisAngleToQuat(axis, delta_angle_rad);
	l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	orientation = l3d_quat_mul(&orientation, q_delta);
	l3d_scene_setObjectOrientation(scene, type, idx, &orientation);

	// Transform the object
	l3d_mat4x4_t mat_rot;
	l3d_quatToRotMat(&mat_rot, q_delta);
	l3d_applyTransformMatrix(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?

	return L3D_OK;
}

l3d_err_t l3d_rotateAboutOriginAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad) {
	// Update object's orientation
	l3d_quat_t q_delta = l3d_axisAngleToQuat(axis, delta_angle_rad);
	l3d_rotateAboutOriginQuat(scene, type, idx, &q_delta);
	// l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	// orientation = l3d_quat_mul(&orientation, &q_delta);
	// l3d_scene_setObjectOrientation(scene, type, idx, &orientation);

	// // Transform the object
	// l3d_mat4x4_t mat_rot;
	// l3d_mat4x4_makeRot(&mat_rot, axis, delta_angle_rad);
	// l3d_applyTransformMatrix(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?

	return L3D_OK;
}

// l3d_err_t l3d_rotateAboutOriginAxisAux(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, uint8_t axis_idx, l3d_rtnl_t delta_angle_rad) {
// 	// Get object's current local rotation value
// 	l3d_rot_t local_rot = l3d_scene_getObjectLocalRot(scene, type, idx);
// 	l3d_mat4x4_t mat_rot;
// 	switch (axis_idx) {
// 		case L3D_AXIS_X:
// 			local_rot.pitch += delta_angle_rad;
// 			l3d_mat4x4_makeRotX(&mat_rot, delta_angle_rad);
// 			break;
// 		case L3D_AXIS_Y:
// 			local_rot.roll += delta_angle_rad;
// 			l3d_mat4x4_makeRotY(&mat_rot, delta_angle_rad);
// 			break;
// 		case L3D_AXIS_Z:
// 			local_rot.yaw += delta_angle_rad;
// 			l3d_mat4x4_makeRotZ(&mat_rot, delta_angle_rad);
// 			break;
// 		default:	// don't transform
// 			return L3D_WRONG_PARAM;
// 	}
// 	// Update object's local rotation value
// 	l3d_scene_setObjectLocalRot(scene, type, idx, &local_rot);
// 	l3d_applyTransformMatrix(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?

// 	return L3D_OK;
// }

l3d_err_t l3d_rotateAboutOriginX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	// return l3d_rotateAboutOriginAxisAux(scene, type, idx, L3D_AXIS_X, delta_angle_rad);
	l3d_rot_t r = l3d_getRotFromFloat(0.0f, delta_angle_rad, 0.0f);
	l3d_quat_t q = l3d_eulerToQuat(&r);
	return l3d_rotateAboutOriginQuat(scene, type, idx, &q);
}

l3d_err_t l3d_rotateAboutOriginY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	// return l3d_rotateAboutOriginAxisAux(scene, type, idx, L3D_AXIS_Y, delta_angle_rad);
	l3d_rot_t r = l3d_getRotFromFloat(0.0f, 0.0f, delta_angle_rad);
	l3d_quat_t q = l3d_eulerToQuat(&r);
	return l3d_rotateAboutOriginQuat(scene, type, idx, &q);
}

l3d_err_t l3d_rotateAboutOriginZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	// return l3d_rotateAboutOriginAxisAux(scene, type, idx, L3D_AXIS_Z, delta_angle_rad);
	l3d_rot_t r = l3d_getRotFromFloat(delta_angle_rad, 0.0f, 0.0f);
	l3d_quat_t q = l3d_eulerToQuat(&r);
	return l3d_rotateAboutOriginQuat(scene, type, idx, &q);
}

l3d_err_t l3d_rotateGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta) {
	// Move the object to the origin (0, 0, 0)
	l3d_vec4_t displacement = l3d_scene_getObjectLocalPos(scene, type, idx);
	displacement = l3d_vec4_negate(&displacement);
	l3d_mat4x4_t mat_transform;
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_applyTransformMatrix(scene, type, idx, &mat_transform);	

	// Rotate the object
	// l3d_rotateAboutOriginQuat(scene, type, idx, q_delta);
	// Update object's orientation
	l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	orientation = l3d_quat_mul(q_delta, &orientation);
	orientation = l3d_quat_normalise(&orientation);
	l3d_scene_setObjectOrientation(scene, type, idx, &orientation);

	// Transform the object
	l3d_mat4x4_t mat_rot;
	l3d_quatToRotMat(&mat_rot, q_delta);
	l3d_applyTransformMatrix(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?

	// Move the object to its initial position
	displacement = l3d_vec4_negate(&displacement);
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_applyTransformMatrix(scene, type, idx, &mat_transform);

	return L3D_OK;
}

l3d_err_t l3d_rotateLocalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta) {
	// // Rotate the object
	// // l3d_rotateAboutOriginQuat(scene, type, idx, q_delta);
	// // Update object's orientation
	// l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	// orientation = l3d_quat_mul(q_delta, &orientation);
	// l3d_scene_setObjectOrientation(scene, type, idx, &orientation);


	// l3d_quat_t initial_rot = l3d_scene_getObjectOrientation(scene, type, idx);
	// l3d_quat_t final_rot = l3d_quat_mul(&initial_rot, q_delta);
	// l3d_quat_t initial_rot_conj = l3d_quat_complexConjugate(&initial_rot);
	// l3d_quat_t local_delta = l3d_quat_mul(&final_rot, &initial_rot_conj);

	// l3d_scene_setObjectOrientation(scene, type, idx, &final_rot);

	// // Transform the object
	// l3d_mat4x4_t mat_rot;
	// l3d_quatToRotMat(&mat_rot, &local_delta);
	// l3d_applyTransformMatrix(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?


	// CHATGPT
	l3d_quat_t q_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	q_orientation = l3d_quat_normalise(&q_orientation);

	// // q_global_delta = q_orientation * q_delta * inv(q_orientation)
	// l3d_quat_t q_inv = l3d_quat_inverse(&q_orientation);
	// l3d_quat_t q_global_delta = l3d_quat_mul(&q_orientation, q_delta);
	// q_global_delta = l3d_quat_mul(&q_global_delta, &q_inv);
	// q_global_delta = l3d_quat_normalise(&q_global_delta);

	// // Mine:
	// // l3d_quat_t q_global_delta = l3d_quat_mul(q_delta, &q_inv);
	// // q_global_delta = l3d_quat_mul(&q_orientation, &q_global_delta);
	// // q_global_delta = l3d_quat_normalise(&q_global_delta);

	// q_orientation = 
	q_orientation = l3d_quat_mul(&q_orientation, q_delta);
	q_orientation = l3d_quat_normalise(&q_orientation);

	l3d_scene_setObjectOrientation(scene, type, idx, &q_orientation);

	// Move the object to the origin (0, 0, 0)
	l3d_vec4_t displacement = l3d_scene_getObjectLocalPos(scene, type, idx);
	displacement = l3d_vec4_negate(&displacement);
	l3d_mat4x4_t mat_transform;
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_applyTransformMatrix(scene, type, idx, &mat_transform);	

	// Transform the object
	l3d_mat4x4_t mat_rot;
	l3d_quatToRotMat(&mat_rot, q_delta);
	l3d_applyTransformMatrix(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?

	// Move the object to its initial position
	displacement = l3d_vec4_negate(&displacement);
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_applyTransformMatrix(scene, type, idx, &mat_transform);

	return L3D_OK;
}

l3d_err_t l3d_rotateGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	// return l3d_rotateGlobalAux(scene, type, idx, 1, delta_angle_rad);
	l3d_rot_t r = l3d_getRotFromFloat(0.0f, delta_angle_rad, 0.0f);
	l3d_quat_t q = l3d_eulerToQuat(&r);
	return l3d_rotateGlobalQuat(scene, type, idx, &q);
}

l3d_err_t l3d_rotateGlobalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	// return l3d_rotateGlobalAux(scene, type, idx, 2, delta_angle_rad);
	l3d_rot_t r = l3d_getRotFromFloat(0.0f, 0.0f, delta_angle_rad);
	l3d_quat_t q = l3d_eulerToQuat(&r);
	return l3d_rotateGlobalQuat(scene, type, idx, &q);
}

l3d_err_t l3d_rotateGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	// return l3d_rotateGlobalAux(scene, type, idx, 3, delta_angle_rad);
	l3d_rot_t r = l3d_getRotFromFloat(delta_angle_rad, 0.0f, 0.0f);
	l3d_quat_t q = l3d_eulerToQuat(&r);
	return l3d_rotateGlobalQuat(scene, type, idx, &q);
}

l3d_err_t l3d_rotateAboutPivotAux(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad, const l3d_vec4_t *pivot) {
	// Object's local rotation is updated in caller function
	// to avoid unnecessary calculations when axis is simple and already known

	// Move the object to the origin (0, 0, 0)
	l3d_vec4_t displacement = *pivot;
	displacement = l3d_vec4_negate(&displacement);
	l3d_mat4x4_t mat_transform;
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_applyTransformMatrix(scene, type, idx, &mat_transform); 				
	// Rotate the object
	if (!l3d_vec4_isNormalised(axis))
		*axis = l3d_vec4_normalise(axis);	// axis should already be a unit vector
	l3d_mat4x4_makeRot(&mat_transform, axis, delta_angle_rad);
	l3d_applyTransformMatrix(scene, type, idx, &mat_transform);
	// Move the object to its initial position
	displacement = l3d_vec4_negate(&displacement);
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_applyTransformMatrix(scene, type, idx, &mat_transform);

	return L3D_OK;
}

// 
// Public function
// Rotate by axis and angle
// axis vector must be normalised
// Update object's local rotation value here
// 
l3d_err_t l3d_rotateAboutPivotAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad, const l3d_vec4_t *pivot) {
	// Update object's orientation
	l3d_quat_t q_delta = l3d_axisAngleToQuat(axis, delta_angle_rad);
	l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	orientation = l3d_quat_mul(&orientation, &q_delta);
	l3d_scene_setObjectOrientation(scene, type, idx, &orientation);

	return l3d_rotateAboutPivotAux(scene, type, idx, axis, delta_angle_rad, pivot);
}

l3d_err_t l3d_rotateAboutAxisAux(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, uint8_t axis_idx, l3d_rtnl_t delta_angle_rad, const l3d_vec4_t *pivot) {
	l3d_obj3d_t *obj = NULL;
	l3d_camera_t *cam = NULL;
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			cam = &scene->cameras[idx];

			// Update object's local rotation value
			switch (axis_idx) {
				case L3D_AXIS_LOCAL_POS:
					return L3D_WRONG_PARAM;
				case L3D_AXIS_X:
					cam->local_rot.pitch += delta_angle_rad;
					break;
				case L3D_AXIS_Y:
					cam->local_rot.roll += delta_angle_rad;
					break;
				case L3D_AXIS_Z:
					cam->local_rot.yaw += delta_angle_rad;
					break;
			}
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			obj = &scene->objects[idx];

			// Update object's local rotation value
			switch (axis_idx) {
				case L3D_AXIS_LOCAL_POS:
					return L3D_WRONG_PARAM;
				case L3D_AXIS_X:
					obj->local_rot.pitch += delta_angle_rad;
					break;
				case L3D_AXIS_Y:
					obj->local_rot.roll += delta_angle_rad;
					break;
				case L3D_AXIS_Z:
					obj->local_rot.yaw += delta_angle_rad;
					break;
			}
			break;
	}

	l3d_vec4_t axis = l3d_scene_getObjectLocalUnitVecIdx(scene, type, idx, axis_idx);
	return l3d_rotateAboutPivotAux(scene, type, idx, &axis, delta_angle_rad, pivot);

	return L3D_OK;
}

l3d_err_t l3d_rotateX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t pivot = l3d_scene_getObjectLocalPos(scene, type, idx);
	return l3d_rotateAboutAxisAux(scene, type, idx, L3D_AXIS_X, delta_angle_rad, &pivot);
}

l3d_err_t l3d_rotateY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t pivot = l3d_scene_getObjectLocalPos(scene, type, idx);
	return l3d_rotateAboutAxisAux(scene, type, idx, L3D_AXIS_Y, delta_angle_rad, &pivot);
}

l3d_err_t l3d_rotateZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t pivot = l3d_scene_getObjectLocalPos(scene, type, idx);
	return l3d_rotateAboutAxisAux(scene, type, idx, L3D_AXIS_Z, delta_angle_rad, &pivot);
}

l3d_err_t l3d_resetRotationGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
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
// Set new orientation to identity quaternion or compute the new rotation not to ignore the error?
// 
l3d_err_t l3d_resetRotationOriginGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx) {
	l3d_quat_t diff = l3d_scene_getObjectOrientation(scene, type, idx);
	diff = l3d_quat_inverse(&diff);
	diff = l3d_quat_normalise(&diff);

	// l3d_quat_t old_orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	// l3d_quat_t product = l3d_quat_mul(&diff, &old_orientation);
	// product = l3d_quat_normalise(&product);
	// L3D_DEBUG_PRINT_QUAT(product);

	// assert(product.w - q_new->w < L3D_EPSILON_RTNL && 
	// 	product.x - q_new->x < L3D_EPSILON_RTNL && 
	// 	product.y - q_new->y < L3D_EPSILON_RTNL && 
	// 	product.z - q_new->z < L3D_EPSILON_RTNL);
	
	l3d_mat4x4_t mat_rot;
	l3d_quatToRotMat(&mat_rot, &diff);
	l3d_applyTransformMatrix(scene, type, idx, &mat_rot);

	// Update object's orientation
	l3d_quat_t qi = l3d_getIdentityQuat();
	l3d_scene_setObjectOrientation(scene, type, idx, &qi);
	// l3d_quat_t orientation = l3d_scene_getObjectOrientation(scene, type, idx);
	// orientation = l3d_quat_mul(&diff, &orientation);
	// l3d_scene_setObjectOrientation(scene, type, idx, &orientation);
	return L3D_OK;
}

l3d_err_t l3d_setRotationGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_new) {
	// Reset object's orientation
	l3d_resetRotationOriginGlobal(scene, type, idx);
	l3d_rotateAboutOriginQuat(scene, type, idx, q_new);
	return L3D_OK;
}

// Results in some strange results when combining quaternions
// l3d_err_t l3d_setRotationGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_new) {
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

l3d_err_t l3d_setRotationGlobalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t angle_rad) {
	l3d_quat_t q = l3d_axisAngleToQuat(axis, angle_rad);
	q = l3d_quat_normalise(&q);
	return l3d_setRotationGlobalQuat(scene, type, idx, &q);
}

l3d_err_t l3d_setRotationGlobalEuler(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_rot_t *r) {
	l3d_quat_t q = l3d_eulerToQuat(r);
	q = l3d_quat_normalise(&q);
	return l3d_setRotationGlobalQuat(scene, type, idx, &q);
}

l3d_err_t l3d_moveGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
	l3d_translateObject(scene, type, idx, delta_pos);
	return L3D_OK;
}

l3d_err_t l3d_moveGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.x = delta_x;
	l3d_translateObject(scene, type, idx, &delta_pos);
	return L3D_OK;
}

l3d_err_t l3d_moveGlobalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_y) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.y = delta_y;
	l3d_translateObject(scene, type, idx, &delta_pos);
	return L3D_OK;
}

l3d_err_t l3d_moveGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_z) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.z = delta_z;
	l3d_translateObject(scene, type, idx, &delta_pos);
	return L3D_OK;
}

l3d_err_t l3d_move(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
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

	l3d_translateObject(scene, type, idx, &v_displacement);

	return L3D_OK;
}

l3d_err_t l3d_moveX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.x = delta_x;
	return l3d_move(scene, type, idx, &delta_pos);
}

l3d_err_t l3d_moveY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_y) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.y = delta_y;
	return l3d_move(scene, type, idx, &delta_pos);
}

l3d_err_t l3d_moveZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_z) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.z = delta_z;
	return l3d_move(scene, type, idx, &delta_pos);
}
