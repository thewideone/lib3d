#ifndef _LIB3D_TRANSFORM_H_
#define _LIB3D_TRANSFORM_H_

// 
// This file contains world space transform functions
// 

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_scene.h"
#include "lib3d_obj3d.h"
#include "lib3d_camera.h"

l3d_err_t l3d_applyTransformMatrix(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_mat4x4_t *mat_transform);
l3d_err_t l3d_additiveTranslateObject(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos);

// 
// Rotate object by quaternion, in world/global space (actually object/local space)
// 

l3d_err_t l3d_rotateGlobalAboutOriginQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta);
l3d_err_t l3d_rotateGlobalAboutPivotQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *pivot, const l3d_quat_t *q_delta);
l3d_err_t l3d_rotateGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta);
// l3d_err_t l3d_rotateLocalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_delta);

// 
// Rotate by axis-angle
// 

l3d_err_t l3d_rotateAboutOriginAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateAboutPivotAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *pivot, const l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateGlobalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateLocalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *local_axis, l3d_rtnl_t delta_angle_rad);

// 
// Rotation about global axes
// 

l3d_err_t l3d_rotateGlobalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateGlobalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateGlobalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);

// 
// Rotation about local axes
// 

l3d_err_t l3d_rotateLocalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateLocalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_rotateLocalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_angle_rad);

// 
// Set / reset orientation
// 

l3d_err_t l3d_resetOrientationGlobal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx);
l3d_err_t l3d_setOrientationGlobalQuat(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_quat_t *q_new);
l3d_err_t l3d_setOrientationGlobalAxisAngle(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_vec4_t *axis, l3d_rtnl_t angle_rad);
l3d_err_t l3d_setOrientationGlobalEuler(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_rot_t *r);

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

l3d_err_t l3d_moveLocal(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *delta_pos);
l3d_err_t l3d_moveLocalX(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_x);
l3d_err_t l3d_moveLocalY(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_y);
l3d_err_t l3d_moveLocalZ(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, l3d_rtnl_t delta_z);

// 
// Set global position
// 

l3d_err_t l3d_setGlobalPos(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *dest);

#endif // _LIB3D_TRANSFORM_H_