// 
// Mathematics needed for the engine
// 

#ifndef _LIB3D_MATH3D_H_
#define _LIB3D_MATH3D_H_

// #include <inttypes.h>   // for int32_t etc.
#include "lib3d_config.h"

#ifdef LIB3D_USE_FIXED_POINT_ARITHMETIC
fxp_t floatToFixed( flp_t num );
flp_t fixedToFloat( fxp_t num );
fxp_t fixedMul( fxp_t a, fxp_t b );
fxp_t fixedDiv( fxp_t a, fxp_t b );
#endif

// Matrix 4x4:
typedef struct {
	rtnl_t m[4][4];// = { 0 };	// rows, columns
} mat4x4_t;

typedef struct {
	rtnl_t x;
	rtnl_t y;
	rtnl_t z;
	rtnl_t h;	// homogeneous coordinates are used
} vec4_t;

typedef struct {
	uint32_t vert_ids[2];
	bool is_visible;	// or partially?
	bool is_boundary;
	bool is_face_outer;
} edge_t;

typedef struct {
	uint32_t vert_ids[3];
	uint32_t edge_ids[3];
	bool is_visible;
} tri_t;

typedef struct {
	tri_t* tris;	// of the original model
	edge_t* edges;				// seems to be dynamic
	vec4_t* vertices_world;		// please make them static ;-;
	vec4_t* vertices_projected;
	uint32_t tri_count;
	uint32_t vert_count;
	uint32_t edge_count;
} mesh_t;

#endif // _LIB3D_MATH3D_H_
