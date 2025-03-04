#ifndef _L3D_MESH_H_
#define _L3D_MESH_H_

#include "lib3d_config.h"
#include "lib3d_math.h"

/* TODO:
 Fix naming conflict: tri_count / face_count
*/

typedef struct {
	// See EDABA "L05-Physical-level-part-1.pptx" slides 12 & 13
	// Maybe put indices of the beginning of each mesh inside the scene struct
	// to allow validation (and program crashes due to wrong memory accesses)?
	uint16_t model_vert_data_offset;
	uint16_t model_tri_data_offset;
	uint16_t model_edge_data_offset;
	
	uint16_t transformed_vertices_offset;
	uint16_t tris_flags_offset;
	uint16_t edges_flags_offset;

	// vec4_t *verts;				// of the original model
	// tri_t *tris;				// 
	// edge_t *edges;				// 
	// vec4_t *verts_world;		// for hidden line removal
	// vec4_t *verts_projected;	// also for hidden line removal?

	uint16_t tri_count;
	uint16_t vert_count;
	uint16_t edge_count;
} l3d_mesh_t;

// uint32_t mesh_loadP(mesh_t *mesh,
// 					const rtnl_t *verts, const uint16_t *faces, const uint16_t *edges,
// 					uint16_t vert_count, uint16_t tri_count, uint16_t edge_count );

#endif // _L3D_MESH_H_