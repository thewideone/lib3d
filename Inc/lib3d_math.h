// 
// Mathematics needed for the engine
// 

#ifndef _L3D_MATH3D_H_
#define _L3D_MATH3D_H_

#include "lib3d_config.h"
#include <inttypes.h>   // for int32_t etc.

// Fixed point arithmetic:
// Functions taken from javidx9's
// "Back To Basics! Fixed Point Numbers in C++" video

// Floating point number type
typedef float l3d_flp_t;
// Fixed point number type
typedef int32_t l3d_fxp_t;
// Twice-wide fixed point number type
typedef int64_t l3d_fxp2_t;
// Number of binary digits after the decimal place
#define L3D_FP_DP 16

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
// Rational number type (fxp_t for fixed point representation)
typedef l3d_fxp_t l3d_rtnl_t;
#else
// Rational number type (flp_t for floating point representation)
typedef l3d_flp_t l3d_rtnl_t;
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

// Is it needed anymore?
// Edge:
// typedef struct {
// 	uint16_t verts_ids[2];
// 	uint16_t tri_id;		// for speed optimization,
// 							// so far used only by depth test;
// 							// ID of first encountered tri the edge belongs to
							
// 	bool is_visible;		// or 2-bit for partial visibility?
// 	bool is_boundary;		// of a face
// 	bool is_silhouette;		// of whole mesh projected onto 2D space
// } l3d_edge_t;

// Is it needed anymore?
// Triangle:
// typedef struct {
// 	uint16_t verts_ids[3];
// 	uint16_t edges_ids[3];	// is it needed?
// 	bool is_visible;
// } l3d_tri_t;

// 
// Converting functions
// 
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
l3d_fxp_t l3d_floatToFixed( l3d_flp_t num );
l3d_flp_t l3d_fixedToFloat( l3d_fxp_t num );
l3d_fxp_t l3d_fixedMul( l3d_fxp_t a, l3d_fxp_t b );
l3d_fxp_t l3d_fixedDiv( l3d_fxp_t a, l3d_fxp_t b );
#endif
l3d_rtnl_t l3d_floatToRational(l3d_flp_t num);
l3d_flp_t l3d_rationalToFloat(l3d_rtnl_t num);
int32_t l3d_rationalToInt32(l3d_rtnl_t num);
l3d_rot_t l3d_getRotFromFloat(l3d_flp_t yaw, l3d_flp_t pitch, l3d_flp_t roll);
l3d_vec4_t l3d_getVec4FromFloat(l3d_flp_t x, l3d_flp_t y, l3d_flp_t z, l3d_flp_t h);

l3d_rtnl_t l3d_getZeroRtnl(void);
l3d_rot_t l3d_getZeroRot(void);
l3d_vec4_t l3d_getZeroVec4(void);

// 
// Below are to be implemented
// 

// 
// Debug
// 
int l3d_printVec4( char *buf, uint16_t buf_size, l3d_vec4_t *v );
int l3d_printMat4x4( char *buf, uint16_t buf_size, l3d_mat4x4_t *m );

// 
// Vector operations
// 
l3d_vec4_t l3d_vec4_add( l3d_vec4_t *v1, l3d_vec4_t *v2 );
l3d_vec4_t l3d_vec4_sub( l3d_vec4_t *v1, l3d_vec4_t *v2 );
l3d_vec4_t l3d_vec4_mul( l3d_vec4_t *v, l3d_rtnl_t k );
l3d_vec4_t l3d_vec4_div( l3d_vec4_t *v, l3d_rtnl_t k );
l3d_rtnl_t l3d_vec4_dotProduct( l3d_vec4_t *v1, l3d_vec4_t *v2 );
l3d_vec4_t l3d_vec4_crossProduct( l3d_vec4_t *v1, l3d_vec4_t *v2 );
l3d_rtnl_t l3d_vec4_length( l3d_vec4_t *v );	// double check: what if length = 0?
l3d_vec4_t l3d_vec4_normalise( l3d_vec4_t *v );

// 
// Matrix operations
// 
l3d_vec4_t l3d_mat4x4_mulVec4( l3d_mat4x4_t *m, l3d_vec4_t *v );

void l3d_mat4x4_makeEmpty( l3d_mat4x4_t *m );
void l3d_mat4x4_makeIdentity( l3d_mat4x4_t *m );

// Rotation matrices generation probably needs to be optimized
void l3d_mat4x4_makeRotZ( l3d_mat4x4_t *m, l3d_rtnl_t angle_rad );
void l3d_mat4x4_makeRotX( l3d_mat4x4_t *m, l3d_rtnl_t angle_rad );
#ifdef L3D_CAMERA_MOVABLE
void l3d_mat4x4_makeRotY( l3d_mat4x4_t *m, l3d_rtnl_t angle_rad);
#endif // L3D_CAMERA_MOVABLE

void l3d_mat4x4_makeTranslation( l3d_mat4x4_t *m, l3d_rtnl_t x, l3d_rtnl_t y, l3d_rtnl_t z );
void l3d_mat4x4_makeProjection( l3d_mat4x4_t *m, l3d_rtnl_t fov_degrees, l3d_rtnl_t aspect_ratio, l3d_rtnl_t near_plane, l3d_rtnl_t far_plane );
void l3d_mat4x4_mulMatrix( l3d_mat4x4_t *m_out, l3d_mat4x4_t *m1, l3d_mat4x4_t *m2 );

#ifdef L3D_CAMERA_MOVABLE
//  pos - where the object should be
//  target - "forward" vector for that object
//  up - "up" vector
void l3d_mat4x4_pointAt( l3d_mat4x4_t *m_out, l3d_vec4_t *pos, l3d_vec4_t *target, l3d_vec4_t *up );

// WIP:
void l3d_mat4x4_lookAtRH( l3d_mat4x4_t *m_out, l3d_vec4_t *eye, l3d_vec4_t *target, l3d_vec4_t *up );
void l3d_mat4x4_lookAtLH( l3d_mat4x4_t *m_out, l3d_vec4_t *eye, l3d_vec4_t *target, l3d_vec4_t *up );
void l3d_mat4x4_FPS( l3d_mat4x4_t *m_out, l3d_vec4_t *pos, l3d_rtnl_t pitch, l3d_rtnl_t yaw );

// Works only for Rotation/Translation Matrices
void l3d_mat4x4_quickInverse( l3d_mat4x4_t *m_out, l3d_mat4x4_t *m );
#endif	// L3D_CAMERA_MOVABLE

#endif // _L3D_MATH3D_H_
