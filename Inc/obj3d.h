// 
// 3D object header file
// 

#ifndef _LIB3D_OBJ3D_H_
#define _LIB3D_OBJ3D_H_

#include "math3d.h"

typedef struct {
	uint32_t tri_count;
} mesh_t;

typedef struct {
	mesh_t mesh;
} obj3d_t;

#endif	// _LIB3D_OBJ3D_H_