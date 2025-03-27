#include "../Inc/lib3d_transform.h"

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

			// cam->has_moved = true;

			// for each child: transform it...
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj3d = &scene->objects[idx];
			if (obj3d == NULL)
				return;
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

			obj3d->has_moved = true;

			// for each child: transform it...
			break;
	}
	
	
}

l3d_err_t l3d_obj3d_rotateOrigin(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad) {
	l3d_mat4x4_t mat_rot;
	l3d_mat4x4_makeRot(&mat_rot, axis, delta_angle_rad);

	// switch (type) {
	// 	case L3D_OBJ_TYPE_CAMERA:
	// 		l3d_camera_t *cam = &scene->cameras[idx];
	// 		cam->local_rot or global_rot? = ...
	// 		break;
	// 	case L3D_OBJ_TYPE_OBJ3D:
	// 		l3d_obj3d_t *obj = &scene->objects[idx];
	// 		obj->local_rot or global_rot? = ...
	// 		break;
	// }
	l3d_transformObject(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?

	return L3D_OK;
}

l3d_err_t l3d_obj3d_rotateOriginX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_mat4x4_t mat_rot;
	l3d_mat4x4_makeRotX(&mat_rot, delta_angle_rad);

	// obj->local_rot or global_rot? = ...
	l3d_transformObject(scene, type, idx, &mat_rot); // move to core/processObject if has_moved?

	return L3D_OK;
}

l3d_err_t l3d_obj3d_rotateGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t displacement;

	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			l3d_camera_t *cam = &scene->cameras[idx];
			cam->local_rot.yaw += delta_angle_rad;
			displacement = cam->local_pos;
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj = &scene->objects[idx];
			obj->local_rot.yaw += delta_angle_rad;
			displacement = obj->local_pos;
			break;
	}

	// Move the object to the origin (0, 0, 0)
	displacement.x = -displacement.x;
	displacement.y = -displacement.y;
	displacement.z = -displacement.z;
	l3d_mat4x4_t mat_transform;
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_transformObject(scene, type, idx, &mat_transform); 				
	// Rotate the object
	l3d_mat4x4_makeRotZ(&mat_transform, delta_angle_rad);
	l3d_transformObject(scene, type, idx, &mat_transform);
	// Move the object to its initial position
	displacement.x = -displacement.x;
	displacement.y = -displacement.y;
	displacement.z = -displacement.z;
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_transformObject(scene, type, idx, &mat_transform);

	return L3D_OK;
}

l3d_err_t l3d_obj3d_rotateZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad) {
	l3d_vec4_t displacement, axis;

	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			l3d_camera_t *cam = &scene->cameras[idx];
			cam->local_rot.yaw += delta_angle_rad;
			displacement = cam->local_pos;
			// axis = cam->u_world[3];
			// cam->u_world[] not implemented yet
			return L3D_WRONG_PARAM;
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj = &scene->objects[idx];
			obj->local_rot.yaw += delta_angle_rad;
			displacement = obj->local_pos;
			axis = obj->u_world[3];
			break;
	}

	// Move the object to the origin (0, 0, 0)
	displacement.x = -displacement.x;
	displacement.y = -displacement.y;
	displacement.z = -displacement.z;
	l3d_mat4x4_t mat_transform;
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_transformObject(scene, type, idx, &mat_transform); 				
	// Rotate the object
	l3d_mat4x4_makeRot(&mat_transform, &axis, delta_angle_rad);
	l3d_transformObject(scene, type, idx, &mat_transform);
	// Move the object to its initial position
	displacement.x = -displacement.x;
	displacement.y = -displacement.y;
	displacement.z = -displacement.z;
	l3d_mat4x4_makeTranslation(&mat_transform, &displacement);
	l3d_transformObject(scene, type, idx, &mat_transform);

	return L3D_OK;
}

l3d_err_t l3d_obj3d_moveGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
	l3d_mat4x4_t mat_translation;
	l3d_mat4x4_makeTranslation(&mat_translation, delta_pos);

	switch (type) {
		case L3D_OBJ_TYPE_CAMERA:
			l3d_camera_t *cam = &scene->cameras[idx];
			cam->local_pos = l3d_vec4_add(&cam->local_pos, delta_pos);
			break;
		case L3D_OBJ_TYPE_OBJ3D:
			l3d_obj3d_t *obj = &scene->objects[idx];
			obj->local_pos = l3d_vec4_add(&obj->local_pos, delta_pos);
			break;
	}

	l3d_transformObject(scene, type, idx, &mat_translation); // move to core/processObject if has_moved?
	return L3D_OK;
}

l3d_err_t l3d_obj3d_moveGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.x = delta_x;
	return l3d_obj3d_moveGlobal(scene, type, idx, &delta_pos);

	// or VV ?

	// l3d_mat4x4_t mat_translation;
	// l3d_vec4_t delta_pos = l3d_getZeroVec4();
	// delta_pos.x = delta_x;
	// l3d_mat4x4_makeTranslation(&mat_translation, &delta_pos);
	
	// obj->local_pos.x += delta_pos.x;

	// l3d_transformObject(scene, type, idx, &mat_translation); // move to core/processObject if has_moved?
	return L3D_OK;
}

l3d_err_t l3d_obj3d_move(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos) {
	// Not implemented yet
	if (type != L3D_OBJ_TYPE_OBJ3D)
		return L3D_WRONG_PARAM;

	l3d_obj3d_t *obj = &scene->objects[idx];

	// Multiply local direction unit vectors by corresponding components of the position delta
	l3d_vec4_t v_displ_x = l3d_vec4_mul(&obj->u_world[1], delta_pos->x);
	l3d_vec4_t v_displ_y = l3d_vec4_mul(&obj->u_world[2], delta_pos->y);
	l3d_vec4_t v_displ_z = l3d_vec4_mul(&obj->u_world[3], delta_pos->z);

	// Combine (add) displacement vectors
	l3d_vec4_t v_displacement = v_displ_x;
	v_displacement = l3d_vec4_add(&v_displacement, &v_displ_y);
	v_displacement = l3d_vec4_add(&v_displacement, &v_displ_z);

	l3d_mat4x4_t mat_translation;
	l3d_mat4x4_makeTranslation(&mat_translation, &v_displacement);
	
	obj->local_pos.x += v_displacement.x;
	obj->local_pos.y += v_displacement.y;
	obj->local_pos.z += v_displacement.z;

	l3d_transformObject(scene, type, idx, &mat_translation); // move to core/processObject if has_moved?

	return L3D_OK;
}

l3d_err_t l3d_obj3d_moveX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x) {
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.x = delta_x;
	return l3d_obj3d_move(scene, type, idx, &delta_pos);

	// or VV ?

	// l3d_mat4x4_t mat_translation;

	// // Multiply the local direction unit vector by the displacement in the axis
	// l3d_vec4_t v_displacement = l3d_vec4_mul(&obj->u_world[1], delta_x);

	// l3d_mat4x4_makeTranslation(&mat_translation, &v_displacement);
	
	// obj->local_pos.x += v_displacement.x;

	// l3d_transformObject(scene, type, idx, &mat_translation); // move to core/processObject if has_moved?
	return L3D_OK;
}
