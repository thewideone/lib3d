#ifndef _LIB3D_TRANSFORM_H_
#define _LIB3D_TRANSFORM_H_

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_scene.h"
#include "lib3d_obj3d.h"
#include "lib3d_camera.h"

// Apply transformation matrix to given object
void l3d_applyTransformMatrix(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_mat4x4_t *mat_transform);
// Use addition instead of multiplication by a matrix
void l3d_translateObject(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos);

// // 
// // Pivot point being the origin of the global coordinate system
// // 

// // Rotation about the origin of the coordinate system

// l3d_err_t l3d_rotateAboutOriginQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta);
// l3d_err_t l3d_rotateAboutOriginAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad);

// // TODO: update local_rot
// // l3d_err_t l3d_rotateAboutOriginAxisAux(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, uint8_t axis_idx, l3d_rtnl_t delta_angle_rad);
// l3d_err_t l3d_rotateAboutOriginX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);
// l3d_err_t l3d_rotateAboutOriginY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);
// l3d_err_t l3d_rotateAboutOriginZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);

// 
// Arbitrary pivot
// 
l3d_err_t l3d_rotateAboutPivotQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad, const l3d_vec4_t *pivot);
l3d_err_t l3d_rotateAboutPivotAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad, const l3d_vec4_t *pivot);

// 
// Pivot point being object's local position
// 

// Rotation about global axes
// l3d_err_t l3d_rotateGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta);
// l3d_err_t l3d_rotateLocalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta);

// Rotate object by quaternion, in world/global space (actually object/local space)
l3d_err_t l3d_rotateQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta);

l3d_err_t l3d_rotateGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateGlobalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);

// Rotation about local axes
l3d_err_t l3d_rotateLocalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *global_axis, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateLocalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateLocalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateLocalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);

// 
// Set orientation
// 

// Reset orientation with respect to current position
l3d_err_t l3d_resetRotationGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx);
// Reset orientation with respect to the origin of the coordinate system
l3d_err_t l3d_resetRotationOriginGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx);
l3d_err_t l3d_setRotationGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_new);
l3d_err_t l3d_setRotationGlobalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t angle_rad);
l3d_err_t l3d_setRotationGlobalEuler(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_rot_t *r);

// 
// Move along global axes
// 

l3d_err_t l3d_moveGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos);
l3d_err_t l3d_moveGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x);
l3d_err_t l3d_moveGlobalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_y);
l3d_err_t l3d_moveGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_z);

// 
// Move along object's local axes
// 

l3d_err_t l3d_move(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos);
l3d_err_t l3d_moveX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x);
l3d_err_t l3d_moveY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_y);
l3d_err_t l3d_moveZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_z);

#endif // _LIB3D_TRANSFORM_H_