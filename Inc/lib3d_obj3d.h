#ifndef _LIB3D_OBJ3D_H_
#define _LIB3D_OBJ3D_H_

// 
// 3D object header file
// 

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_mesh.h"

typedef struct {
	mesh_t mesh;
	colour_t wireframe_colour;
	colour_t fill_colour;
	vec4_t local_pos;
	rot_t rot;
	// To be implemented:
	// uint8_t group;
	// obj3d_t* parent;
	// obj3d_t** children;
	// uint8_t children_count; 
} obj3d_t;

#endif	// _LIB3D_OBJ3D_H_