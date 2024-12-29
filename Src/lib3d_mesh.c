#include "../Inc/lib3d_mesh.h"
#include "stdlib.h"

/*
State how big scene can be. If we can't state that, we can't rasterize the scene.

Put all vertices, faces and edges together into their buffers.
Each mesh would then access the data by indexes in buffers.
This allows to avoid memory fragmentation.
*/

// uint32_t mesh_loadP(mesh_t *mesh,
// 					const rtnl_t *verts, const uint16_t *faces, const uint16_t *edges,
// 					uint16_t vert_count, uint16_t tri_count, uint16_t edge_count) {
	
// 	// if( mesh->vert_count != 0 || mesh->tri_count != 0 || mesh->edge_count != 0 || mesh->tris != NULL )

// 	if (mesh->verts != NULL || mesh->tris != NULL || mesh->edges != NULL)
// 		return LIB3D_ERROR_ALREADY_ASSIGNED;


	 

// 	if (mesh->verts == NULL) {
// 		return LIB3D_ERROR_NO_MEMORY;
// 	}
	
// 	mesh->vert_count = vert_count;
// 	mesh->tris = (tri_t*) malloc( tri_count*sizeof(tri_t) );

// 	if (mesh->tris == NULL) {
// 		free(mesh->verts);
// 		mesh->verts = NULL;
// 		mesh->vert_count = 0;
// 		return LIB3D_ERROR_NO_MEMORY;
// 	}

// 	mesh->tri_count = tri_count;
// 	mesh->edges = (edge_t*) malloc( edge_count*sizeof(edge_t) );

// 	if (mesh->edges == NULL) {
// 		free(mesh->verts);
// 		mesh->verts = NULL;
// 		mesh->vert_count = 0;
// 		free(mesh->tris);
// 		mesh->tris = NULL;
// 		mesh->tri_count = 0;
// 		return LIB3D_ERROR_NO_MEMORY;
// 	}

// 	mesh->edge_count = edge_count;
	
// }