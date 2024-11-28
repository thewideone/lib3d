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

// Rotation struct:
typedef struct {
	rtnl_t yaw;
	rtnl_t pitch;
	rtnl_t roll;
} rot_t;

// Matrix 4x4:
typedef struct {
	rtnl_t m[4][4];	// rows, columns
} mat4x4_t;

// 4D vector:
typedef struct {
	rtnl_t x;
	rtnl_t y;
	rtnl_t z;
	rtnl_t h;	// homogeneous coordinates are used
} vec4_t;

// Edge:
typedef struct {
	uint16_t verts_ids[2];
	bool is_visible;		// or 2-bit for partial visibility?
	bool is_boundary;		// or 1 bit
	bool is_face_outer;		// or 1 bit
} edge_t;

// Triangle:
typedef struct {
	uint16_t verts_ids[3];
	uint16_t edges_ids[3];
	bool is_visible;
} tri_t;



#endif // _LIB3D_MATH3D_H_
