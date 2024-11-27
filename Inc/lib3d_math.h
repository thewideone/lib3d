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
	rtnl_t h;
} vec4_t;

#endif // _LIB3D_MATH3D_H_
