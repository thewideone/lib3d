#ifndef _LIB3D_MESH_H_
#define _LIB3D_MESH_H_

#include "lib3d_config.h"
#include "lib3d_math.h"

/* TODO:
 Fix naming conflict: tri_count / face_count
*/

typedef struct {
	vec4_t *verts;				// of the original model
	tri_t *tris;				// 
	edge_t *edges;				// 
	vec4_t *verts_world;		// for hidden line removal
	vec4_t *verts_projected;	// also for hidden line removal?
	uint16_t tri_count;
	uint16_t vert_count;
	uint16_t edge_count;
} mesh_t;

uint32_t mesh_loadP(mesh_t *mesh,
					const rtnl_t *verts, const uint16_t *faces, const uint16_t *edges,
					uint16_t vert_count, uint16_t tri_count, uint16_t edge_count );

#endif // _LIB3D_MESH_H_