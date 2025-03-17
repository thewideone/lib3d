#ifndef _LIB3D_TRANSFORM_H_
#define _LIB3D_TRANSFORM_H_

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_scene.h"
#include "lib3d_obj3d.h"
#include "lib3d_camera.h"

void l3d_transformObject(l3d_scene_t *scene, l3d_obj3d_t *obj3d, const l3d_mat4x4_t *mat_transform);

// 
// Pivot point being the origin of the global coordinate system
// 

// Rotation about global coordinate system
l3d_err_t l3d_obj3d_rotate(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_vec4_t *axis, l3d_rtnl_t delta_angle_rad);

// 
// Pivot point being object's local position
// 

// Rotation about global coordinate system
// l3d_err_t l3d_obj3d_rotateXGlobal(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_angle_rad);
// l3d_err_t l3d_obj3d_rotateYGlobal(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_obj3d_rotateZGlobal(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_angle_rad);

// Rotation about local coordinate system
// l3d_err_t l3d_obj3d_rotateX(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_angle_rad);
// l3d_err_t l3d_obj3d_rotateY(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_angle_rad);
l3d_err_t l3d_obj3d_rotateZ(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_angle_rad);



l3d_err_t l3d_obj3d_move(l3d_scene_t *scene, l3d_obj3d_t *obj, const l3d_vec4_t *delta_pos);
// l3d_err_t l3d_obj3d_moveX(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_x);
// l3d_err_t l3d_obj3d_moveY(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_y);
// l3d_err_t l3d_obj3d_moveZ(l3d_scene_t *scene, l3d_obj3d_t *obj, l3d_rtnl_t delta_z);

#endif // _LIB3D_TRANSFORM_H_