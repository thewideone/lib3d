#include "mesh_pyramid_tri.h"

const fxp_t mesh_pyramid_tri_verts[] = {
	65536, 65536, -65536,
	65536, -65536, -65536,
	0, 0, 65536,
	-65536, 65536, -65536,
	-65536, -65536, -65536
};

const uint16_t mesh_pyramid_tri_faces[] = {
	1, 2, 4,
	4, 2, 3,
	0, 4, 3,
	0, 2, 1,
	3, 2, 0,
	0, 1, 4
};

const uint16_t mesh_pyramid_tri_edges[] = {
	1, 2, 0,
	2, 4, 0,
	1, 4, 0,
	2, 3, 1,
	4, 3, 1,
	0, 4, 2,
	0, 3, 2,
	0, 2, 3,
	0, 1, 3
};

uint8_t mesh_pyramid_tri_edge_flags[] = {
	6,
	6,
	6,
	6,
	6,
	4,
	6,
	6,
	6
};

