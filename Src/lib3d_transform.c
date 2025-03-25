#include "../Inc/lib3d_transform.h"

// 
// This function applies given transformation matrix to given object in world space
// 
void l3d_transformObject(l3d_scene_t *scene, l3d_obj3d_t *obj3d, const l3d_mat4x4_t *mat_transform) {
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
}

l3d_err_t l3d_obj3d_rotateOrigin(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad) {
	l3d_mat4x4_t mat_rot;
	l3d_mat4x4_makeRot(&mat_rot, axis, delta_angle_rad);

	// obj->local_rot or global_rot? = ...
	obj->has_moved = true;
	l3d_transformObject(scene, obj, &mat_rot); // move to core/processObject if has_moved?

	return L3D_OK;
}

l3d_err_t l3d_obj3d_rotateOriginX(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_angle_rad) {
	l3d_mat4x4_t mat_rot;
	l3d_mat4x4_makeRotX(&mat_rot, delta_angle_rad);

	// obj->local_rot or global_rot? = ...
	obj->has_moved = true;
	l3d_transformObject(scene, obj, &mat_rot); // move to core/processObject if has_moved?

	return L3D_OK;
}

l3d_err_t l3d_obj3d_rotateGlobalZ(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_angle_rad) {
	l3d_mat4x4_t mat_transform;
	l3d_vec4_t local_pos_negative;

	local_pos_negative.x = -obj->local_pos.x;
	local_pos_negative.y = -obj->local_pos.y;
	local_pos_negative.z = -obj->local_pos.z;

	// Move the object to the origin (0, 0, 0)
	l3d_mat4x4_makeTranslation(&mat_transform, &local_pos_negative);
	l3d_transformObject(scene, obj, &mat_transform); 				
	// Rotate the object
	l3d_mat4x4_makeRotZ(&mat_transform, delta_angle_rad);
	l3d_transformObject(scene, obj, &mat_transform);
	// Move the object to its initial position
	l3d_mat4x4_makeTranslation(&mat_transform, &obj->local_pos);
	l3d_transformObject(scene, obj, &mat_transform);

	// obj->local_rot or global_rot? = ...
	obj->local_rot.yaw += delta_angle_rad;

	obj->has_moved = true;

	return L3D_OK;
}

l3d_err_t l3d_obj3d_rotateZ(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_angle_rad) {
	l3d_mat4x4_t mat_transform;
	l3d_vec4_t local_pos_negative;

	local_pos_negative.x = -obj->local_pos.x;
	local_pos_negative.y = -obj->local_pos.y;
	local_pos_negative.z = -obj->local_pos.z;

	// Move the object to the origin (0, 0, 0)
	l3d_mat4x4_makeTranslation(&mat_transform, &local_pos_negative);
	l3d_transformObject(scene, obj, &mat_transform); 				
	// Rotate the object
	l3d_mat4x4_makeRot(&mat_transform, &obj->u_world[3], delta_angle_rad);
	l3d_transformObject(scene, obj, &mat_transform);
	// Move the object to its initial position
	l3d_mat4x4_makeTranslation(&mat_transform, &obj->local_pos);
	l3d_transformObject(scene, obj, &mat_transform);

	// obj->local_rot or global_rot? = ...
	obj->local_rot.yaw += delta_angle_rad;

	obj->has_moved = true;

	return L3D_OK;
}

l3d_err_t l3d_obj3d_moveGlobal(l3d_scene_t *scene, l3d_obj3d_t *obj, const l3d_vec4_t *delta_pos) {
	l3d_mat4x4_t mat_translation;
	l3d_mat4x4_makeTranslation(&mat_translation, delta_pos);
	
	obj->local_pos.x += delta_pos->x;
	obj->local_pos.y += delta_pos->y;
	obj->local_pos.z += delta_pos->z;

	obj->has_moved = true;

	l3d_transformObject(scene, obj, &mat_translation); // move to core/processObject if has_moved?
	return L3D_OK;
}

l3d_err_t l3d_obj3d_moveGlobalX(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_x) {
	// l3d_vec4_t delta_pos = l3d_getZeroVec4();
	// delta_pos.x = delta_x;
	// return l3d_obj3d_moveGlobal(scene, obj, &delta_pos);

	// or VV ?

	l3d_mat4x4_t mat_translation;
	l3d_vec4_t delta_pos = l3d_getZeroVec4();
	delta_pos.x = delta_x;
	l3d_mat4x4_makeTranslation(&mat_translation, &delta_pos);
	
	obj->local_pos.x += delta_pos.x;

	obj->has_moved = true;

	l3d_transformObject(scene, obj, &mat_translation); // move to core/processObject if has_moved?
	return L3D_OK;
}

l3d_err_t l3d_obj3d_move(l3d_scene_t *scene, l3d_obj3d_t *obj, const l3d_vec4_t *delta_pos) {


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

	obj->has_moved = true;

	l3d_transformObject(scene, obj, &mat_translation); // move to core/processObject if has_moved?
	return L3D_OK;
}

l3d_err_t l3d_obj3d_moveX(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_x) {
	// l3d_vec4_t delta_pos = l3d_getZeroVec4();
	// delta_pos.x = delta_x;
	// return l3d_obj3d_move(scene, obj, &delta_pos);

	// or VV ?

	l3d_mat4x4_t mat_translation;

	// Multiply the local direction unit vector by the displacement in the axis
	l3d_vec4_t v_displacement = l3d_vec4_mul(&obj->u_world[1], delta_x);

	l3d_mat4x4_makeTranslation(&mat_translation, &v_displacement);
	
	obj->local_pos.x += v_displacement.x;

	obj->has_moved = true;

	l3d_transformObject(scene, obj, &mat_translation); // move to core/processObject if has_moved?
	return L3D_OK;
}
