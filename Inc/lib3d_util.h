#ifndef _L3D_UTIL_H_
#define _L3D_UTIL_H_

// 
// This file contains utilities for the rest of this library.
// 

#include "lib3d_config.h"

// 
// Draw a line on the screen.
// Used by l3d_drawMesh().
// 
void l3d_drawLineCallback( int32_t x0, int32_t y0, int32_t x1, int32_t y1, l3d_colour_t colour);

// 
// Just for debug.
// Should not be used as regular text-drawing function,
// because that one should be provided by a display library used.
// 
void l3d_putTextCallback( int32_t x, int32_t y, char* str, l3d_colour_t colour);
void l3d_putUInt32Callback( int32_t x, int32_t y, uint32_t num, uint8_t digits_cnt, l3d_colour_t colour );
void l3d_putInt32Callback( int32_t x, int32_t y, int32_t num, uint8_t digits_cnt, l3d_colour_t colour );
// void l3d_putRationalCallback( int32_t x, int32_t y, l3d_rtnl_t num, l3d_colour_t colour);

void l3d_errorHandler(void);

// 
// Debug:
// 

#if defined(L3D_DEBUG) && L3D_DEBUG > 0
#include <stdio.h>	// for fprintf(), sprintf()
// Usage:
//  L3D_DEBUG_PRINT( "Setup complete %d\n", (int) 123 );
 #define L3D_DEBUG_PRINT(fmt, args...) fprintf(stderr, "L3D_DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
 #define L3D_DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif	// defined(L3D_DEBUG) && L3D_DEBUG > 0

// Taken from https://stackoverflow.com/questions/1623111/programmatic-way-to-get-variable-name-in-c
#define L3D_DEBUG_PRINT_FLP(n) L3D_DEBUG_PRINT("%s: %.3f\n", \
					#n, \
                    n)

#define L3D_DEBUG_PRINT_FXP(n) L3D_DEBUG_PRINT("%s: %.3f\n", \
					#n, \
                    l3d_fixedToFloat(n))

#define L3D_DEBUG_PRINT_RTNL(n) L3D_DEBUG_PRINT("%s: %.3f\n", \
					#n, \
                    l3d_rationalToFloat(n))

// #v - l3d_vec4_t variable name
#define L3D_DEBUG_PRINT_VEC4(v) L3D_DEBUG_PRINT("%s: {%.3f, %.3f, %.3f, %.3f}\n", \
					#v, \
                    l3d_rationalToFloat(v.x), l3d_rationalToFloat(v.y), \
                    l3d_rationalToFloat(v.z), l3d_rationalToFloat(v.h))

// #r - l3d_rot_t variable name
#define L3D_DEBUG_PRINT_ROT(r) L3D_DEBUG_PRINT("%s (rad): {y: %.3f, p: %.3f, r: %.3f}\n", \
					#r, \
                    l3d_rationalToFloat(r.yaw), l3d_rationalToFloat(r.pitch), \
                    l3d_rationalToFloat(r.roll))
// #r - l3d_rot_t variable name
#define L3D_DEBUG_PRINT_ROT_DEG(r) L3D_DEBUG_PRINT("%s (deg): {y: %.3f, p: %.3f, r: %.3f}\n", \
    #r, \
    l3d_rationalToFloat(l3d_radToDeg(r.yaw)), l3d_rationalToFloat(l3d_radToDeg(r.pitch)), \
    l3d_rationalToFloat(l3d_radToDeg(r.roll)))

// #mat - l3d_mat4x4_t variable name
#define L3D_DEBUG_PRINT_MAT4X4(mat) L3D_DEBUG_PRINT("%s:\n|%.3f, %.3f, %.3f, %.3f|\n|%.3f, %.3f, %.3f, %.3f|\n|%.3f, %.3f, %.3f, %.3f|\n|%.3f, %.3f, %.3f, %.3f|\n", \
                    #mat, \
					l3d_rationalToFloat(mat.m[0][0]), l3d_rationalToFloat(mat.m[0][1]), l3d_rationalToFloat(mat.m[0][2]), l3d_rationalToFloat(mat.m[0][3]), \
                    l3d_rationalToFloat(mat.m[1][0]), l3d_rationalToFloat(mat.m[1][1]), l3d_rationalToFloat(mat.m[1][2]), l3d_rationalToFloat(mat.m[1][3]), \
                    l3d_rationalToFloat(mat.m[2][0]), l3d_rationalToFloat(mat.m[2][1]), l3d_rationalToFloat(mat.m[2][2]), l3d_rationalToFloat(mat.m[2][3]), \
                    l3d_rationalToFloat(mat.m[3][0]), l3d_rationalToFloat(mat.m[3][1]), l3d_rationalToFloat(mat.m[3][2]), l3d_rationalToFloat(mat.m[3][3]))

#endif	// _L3D_UTIL_H_
