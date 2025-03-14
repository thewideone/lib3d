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
	// obj3d_t* parent;
	// obj3d_t** children;
	// uint8_t children_count; 
	// uint8_t group;

	l3d_colour_t wireframe_colour;
	l3d_colour_t fill_colour;
	bool visible;
	
} l3d_obj3d_t;

// uint32_t obj3d_loadP( obj3d_t* obj, rtnl_t* mesh_verts, uint16_t* mesh_faces, uint16_t* mesh_edges );

// getAbsPos(): recurse through all parents
// l3d_vec4_t l3d_obj3d_getAbsPos(l3d_obj3d_t *obj);
// getAbsRot(): recurse through all parents
// l3d_rot_t l3d_obj3d_getAbsRot(l3d_obj3d_t *obj);

#endif	// _L3D_OBJ3D_H_