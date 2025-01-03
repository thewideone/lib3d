// 
// Mathematics needed for the engine
// 

#ifndef _L3D_MATH3D_H_
#define _L3D_MATH3D_H_

// #include <inttypes.h>   // for int32_t etc.
#include "lib3d_config.h"

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
l3d_fxp_t l3d_floatToFixed( l3d_flp_t num );
l3d_flp_t l3d_fixedToFloat( l3d_fxp_t num );
l3d_fxp_t l3d_fixedMul( l3d_fxp_t a, l3d_fxp_t b );
l3d_fxp_t l3d_fixedDiv( l3d_fxp_t a, l3d_fxp_t b );
#endif

// Rotation struct:
typedef struct {
	l3d_rtnl_t yaw;
	l3d_rtnl_t pitch;
	l3d_rtnl_t roll;
} l3d_rot_t;

// Matrix 4x4:
typedef struct {
	l3d_rtnl_t m[4][4];	// rows, columns
} l3d_mat4x4_t;

// 4D vector:
typedef struct {
	l3d_rtnl_t x;
	l3d_rtnl_t y;
	l3d_rtnl_t z;
	l3d_rtnl_t h;	// homogeneous coordinates are used
} l3d_vec4_t;

// Edge:
typedef struct {
	uint16_t verts_ids[2];
	uint16_t tri_id;		// for speed optimization,
							// so far used only by depth test;
							// ID of first encountered tri the edge belongs to
							
	bool is_visible;		// or 2-bit for partial visibility?
	bool is_boundary;		// of a face
	bool is_silhouette;		// of whole mesh projected onto 2D space
} l3d_edge_t;

// Triangle:
typedef struct {
	uint16_t verts_ids[3];
	uint16_t edges_ids[3];	// is it needed?
	bool is_visible;
} l3d_tri_t;

l3d_rot_t l3d_getRotFromFloat(l3d_flp_t yaw, l3d_flp_t pitch, l3d_flp_t roll);
l3d_vec4_t l3d_getVec4FromFloat(l3d_flp_t x, l3d_flp_t y, l3d_flp_t z, l3d_flp_t h);

l3d_rtnl_t l3d_getZeroRtnl(void);
l3d_rot_t l3d_getZeroRot(void);
l3d_vec4_t l3d_getZeroVec4(void);

l3d_rtnl_t l3d_floatToRational(l3d_flp_t num);
int32_t l3d_rationalToInt32(l3d_rtnl_t num);


#endif // _L3D_MATH3D_H_
