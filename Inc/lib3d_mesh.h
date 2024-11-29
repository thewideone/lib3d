#ifndef _LIB3D_MESH_H_
#define _LIB3D_MESH_H_

#include "lib3d_config.h"
#include "lib3d_math.h"

typedef struct {
	tri_t* tris;				// of the original model
	edge_t* edges;				// 
	vec4_t* verts_world;		// for hidden line removal
	vec4_t* verts_projected;	// also for hidden line removal?
	uint16_t tri_count;
	uint16_t vert_count;
	uint16_t edge_count;
} mesh_t;

#endif // _LIB3D_MESH_H_