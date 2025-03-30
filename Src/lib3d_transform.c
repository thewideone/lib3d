#include "../Inc/lib3d_transform.h"
#include "../Inc/lib3d_util.h"	// for debug print

// 
// This function applies given transformation matrix to given object in world space
// 
void l3d_transformObject(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_mat4x4_t *mat_transform) {
	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			l3d_camera_t *cam = &scene->cameras[idx];
			if (cam == NULL)
				return;
			cam->local_pos = l3d_mat4x4_mulVec4(mat_transform, &cam->local_pos);

			// Transform orientation markers
			cam->u_world[0] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[0]);
			cam->u_world[1] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[1]);
			cam->u_world[2] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[2]);
			cam->u_world[3] = l3d_mat4x4_mulVec4(mat_transform, &cam->u_world[3]);

			cam->updated = true;

			// cam->has_moved = true;

			// for each child: transform it... really here or in the caller function?
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return;
			
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

			// for each child: transform it...
			break;
	}
}

l3d_err_t l3d_obj3d_rotateOrigin(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad) {
	// Update object's local_rot value
	// - TODO

	// Transform the object
	l3d_mat4x4_t mat_rot;
	l3d_mat4x4_makeRot(&mat_rot, axis, delta_angle_rad);
	l3d_transformObject(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?

	return L3D_OK;
}

l3d_err_t l3d_obj3d_rotateOriginAux(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, uint8_t axis_idx, l3d_rtnl_t delta_angle_rad) {
	// Get object's current local rotation value
	l3d_rot_t local_rot = l3d_scene_getObjectLocalRot(scene, type, idx);
	l3d_mat4x4_t mat_rot;
	switch (axis_idx) {
		case 1:	// X
			local_rot.pitch += delta_angle_rad;
			l3d_mat4x4_makeRotX(&mat_rot, delta_angle_rad);
			break;
		case 2:	// Y
			local_rot.roll += delta_angle_rad;
			l3d_mat4x4_makeRotY(&mat_rot, delta_angle_rad);
			break;
		case 3:	// Z
			local_rot.yaw += delta_angle_rad;
			l3d_mat4x4_makeRotZ(&mat_rot, delta_angle_rad);
			break;
		default:	// don't transform
			return L3D_WRONG_PARAM;
	}
	// Update object's local rotation value
	l3d_scene_setObjectLocalRot(scene, type, idx, &local_rot);
	l3d_transformObject(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?

	return L3D_OK;
}

l3d_err_t l3d_obj3d_rotateOriginX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	return l3d_obj3d_rotateOriginAux(scene, type, idx, 1, delta_angle_rad);
}

l3d_err_t l3d_obj3d_rotateOriginY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	return l3d_obj3d_rotateOriginAux(scene, type, idx, 2, delta_angle_rad);
}

l3d_err_t l3d_obj3d_rotateOriginZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	return l3d_obj3d_rotateOriginAux(scene, type, idx, 3, delta_angle_rad);
}

l3d_err_t l3d_obj3d_rotateGlobalAux(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, uint8_t axis_idx, l3d_rtnl_t delta_angle_rad) {
	// Move the object to the origin (0, 0, 0)
	l3d_vec4_t displacement = l3d_scene_getObjectLocalPos(scene, type, idx);
	displacement = l3d_vec4_negate(&displacement);
	l3d_mat4x4_t mat_transform;
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_transformObject(scene, type, idx, &mat_transform);	

	// Rotate the object
	// Update object's local rotation value
	l3d_rot_t local_rot = l3d_scene_getObjectLocalRot(scene, type, idx);
	switch (axis_idx) {
		case 1:	// X
			local_rot.pitch += delta_angle_rad;
			l3d_mat4x4_makeRotX(&mat_transform, delta_angle_rad);
			break;
		case 2:	// Y
			local_rot.roll += delta_angle_rad;
			l3d_mat4x4_makeRotY(&mat_transform, delta_angle_rad);
			break;
		case 3:	// Z
			local_rot.yaw += delta_angle_rad;
			l3d_mat4x4_makeRotZ(&mat_transform, delta_angle_rad);
			break;
		default:	// don't rotate
			break;
	}
	l3d_scene_setObjectLocalRot(scene, type, idx, &local_rot);

	l3d_transformObject(scene, type, idx, &mat_transform);

	// Move the object to its initial position
	displacement = l3d_vec4_negate(&displacement);
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_transformObject(scene, type, idx, &mat_transform);

	return L3D_OK;
}

l3d_err_t l3d_obj3d_rotateGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	return l3d_obj3d_rotateGlobalAux(scene, type, idx, 1, delta_angle_rad);
}

l3d_err_t l3d_obj3d_rotateGlobalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	return l3d_obj3d_rotateGlobalAux(scene, type, idx, 2, delta_angle_rad);
}

l3d_err_t l3d_obj3d_rotateGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	return l3d_obj3d_rotateGlobalAux(scene, type, idx, 3, delta_angle_rad);
}

l3d_err_t l3d_obj3d_rotateAux(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, uint8_t axis_idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t axis;

	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			l3d_camera_t *cam = &scene->cameras[idx];
			axis = l3d_vec4_sub(&cam->u_world[axis_idx], &cam->u_world[0]);	// local Z-axis unit vector
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj = &scene->objects[idx];
			axis = l3d_vec4_sub(&obj->u_world[axis_idx], &obj->u_world[0]);	// local Z-axis unit vector
			break;
	}

	// Update object's local rotation value
	l3d_rot_t local_rot = l3d_scene_getObjectLocalRot(scene, type, idx);
	local_rot.yaw += delta_angle_rad;
	l3d_scene_setObjectLocalRot(scene, type, idx, &local_rot);

	// Move the object to the origin (0, 0, 0)
	l3d_vec4_t displacement = l3d_scene_getObjectLocalPos(scene, type, idx);
	displacement = l3d_vec4_negate(&displacement);
	l3d_mat4x4_t mat_transform;
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_transformObject(scene, type, idx, &mat_transform); 				
	// Rotate the object
	axis = l3d_vec4_normalise(&axis);	// axis should already be a unit vector, but just in case
	l3d_mat4x4_makeRot(&mat_transform, &axis, delta_angle_rad);
	l3d_transformObject(scene, type, idx, &mat_transform);
	// Move the object to its initial position
	displacement = l3d_vec4_negate(&displacement);
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_transformObject(scene, type, idx, &mat_transform);

	return L3D_OK;
}

// TODO: change 1, 2, 3 to some enum
l3d_err_t l3d_obj3d_rotateX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	return l3d_obj3d_rotateAux(scene, type, idx, 1, delta_angle_rad);
}

l3d_err_t l3d_obj3d_rotateY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	return l3d_obj3d_rotateAux(scene, type, idx, 2, delta_angle_rad);
}

l3d_err_t l3d_obj3d_rotateZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	return l3d_obj3d_rotateAux(scene, type, idx, 3, delta_angle_rad);
}

l3d_err_t l3d_obj3d_moveGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
	// Transform the object
	l3d_mat4x4_t mat_translation;
	l3d_mat4x4_makeTranslation(&mat_translation, delta_pos);
	l3d_transformObject(scene, type, idx, &mat_translation); // move to core/processObject if has_moved?
	return L3D_OK;
}

l3d_err_t l3d_obj3d_moveGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.x = delta_x;
	return l3d_obj3d_moveGlobal(scene, type, idx, &delta_pos);
}

l3d_err_t l3d_obj3d_moveGlobalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_y) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.y = delta_y;
	return l3d_obj3d_moveGlobal(scene, type, idx, &delta_pos);
}

l3d_err_t l3d_obj3d_moveGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_z) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.z = delta_z;
	return l3d_obj3d_moveGlobal(scene, type, idx, &delta_pos);
}

l3d_err_t l3d_obj3d_move(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
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

	// Transform the object
	l3d_mat4x4_t mat_translation;
	l3d_mat4x4_makeTranslation(&mat_translation, &v_displacement);
	l3d_transformObject(scene, type, idx, &mat_translation); // move to core/processObject if has_moved?

	return L3D_OK;
}

l3d_err_t l3d_obj3d_moveX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.x = delta_x;
	return l3d_obj3d_move(scene, type, idx, &delta_pos);
}

l3d_err_t l3d_obj3d_moveY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_y) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.y = delta_y;
	return l3d_obj3d_move(scene, type, idx, &delta_pos);
}

l3d_err_t l3d_obj3d_moveZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_z) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.z = delta_z;
	return l3d_obj3d_move(scene, type, idx, &delta_pos);
}