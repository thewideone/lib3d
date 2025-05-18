#ifndef _L3D_CAMERA_H_
#define _L3D_CAMERA_H_

#include "lib3d_config.h"
#include "lib3d_math.h"

typedef enum l3d_camera_type_ {
    L3D_CAMERA_TYPE_FIRST_PERSON,
    L3D_CAMERA_TYPE_CAROUSEL
} l3d_camera_type_t;

typedef struct {
	// Object properties:
	l3d_vec4_t local_pos;
	l3d_quat_t orientation;

	// Object orientation markers (local coordinate system vectors)
	// Can be drawn
	// Probably will be used for object gizmos
	// These four stand for origin, x, y, and z respectively
	l3d_vec4_t u[4];
	l3d_vec4_t u_world[4];
	l3d_vec4_t u_proj[4];
	
	// To be implemented:
	// obj3d_t* parent;
	// obj3d_t** children;
	// uint8_t children_count; 
	// uint8_t group;

	l3d_vec4_t local_look_dir;
	l3d_vec4_t local_up_dir;

	l3d_rtnl_t fov;
	l3d_rtnl_t near_plane;
	l3d_rtnl_t far_plane;

	l3d_camera_type_t type;	// not used and to be removed I guess

	bool has_moved;	// see description in the obj3d struct
	bool is_modified;

} l3d_camera_t;

// l3d_vec4_t l3d_cam_getAbsPos(l3d_camera_t *cam);
// l3d_rot_t l3d_cam_getAbsRot(l3d_camera_t *cam);

l3d_err_t l3d_cam_reset(l3d_camera_t *cam);

#endif // _L3D_CAMERA_H_