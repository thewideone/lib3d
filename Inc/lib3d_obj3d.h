#ifndef _L3D_OBJ3D_H_
#define _L3D_OBJ3D_H_

// 
// 3D object header file
// 

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_mesh.h"

typedef struct {
	l3d_mesh_t mesh;

	// Object properties:
	l3d_vec4_t local_pos;
	l3d_rot_t local_rot;

	// Object orientation markers (local coordinate system vectors)
	// Can be drawn
	// Probably will be used for object gizmos
	// These four stand for origin, x, y, and z respectively
	l3d_vec4_t u[4];
	l3d_vec4_t u_world[4];
	l3d_vec4_t u_proj[4];

	// To be implemented:
	// entity_t* parent;
	// entity_t children[MAX_CHILDREN]; // entity = {type enum, void *entity}
	// uint8_t children_count; 
	// uint8_t group;

	l3d_colour_t wireframe_colour;
	l3d_colour_t fill_colour;
	bool visible;
	bool has_moved;
	
} l3d_obj3d_t;

// Transform functions moved to lib3d_transform.h
// l3d_err_t l3d_obj3d_setGlobalRotation(l3d_obj3d_t *obj, l3d_vec4_t *axis, l3d_rtnl_t angle_rad);
// l3d_err_t l3d_obj3d_setRotation(l3d_obj3d_t *obj, l3d_vec4_t *axis, l3d_rtnl_t angle_rad);

// l3d_err_t l3d_obj3d_rotate(l3d_obj3d_t *obj, l3d_vec4_t *axis, l3d_rtnl_t angle_delta);

// getAbsPos(): recurse through all parents
// l3d_vec4_t l3d_obj3d_getAbsPos(l3d_obj3d_t *obj);
// getAbsRot(): recurse through all parents
// l3d_rot_t l3d_obj3d_getAbsRot(l3d_obj3d_t *obj);

#endif	// _L3D_OBJ3D_H_