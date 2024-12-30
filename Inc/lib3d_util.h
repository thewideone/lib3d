#ifndef _L3D_UTIL_H_
#define _L3D_UTIL_H_

// 
// This file contains utilities for the rest of this library.
// 

#include "lib3d_config.h"

// 
// Draw a line on the screen.
// Used by lib3d_drawMesh().
// 
void lib3d_drawLineCallback( l3d_rtnl_t x0, l3d_rtnl_t y0, l3d_rtnl_t x1, l3d_rtnl_t y1, l3d_colour_t colour);

// 
// Just for debug.
// Should not be used as regular text-drawing function,
// because that one should be provided by a display library used.
// 
void lib3d_putTextCallback( char* str, l3d_rtnl_t x, l3d_rtnl_t y, l3d_colour_t colour);
void lib3d_putUInt32Callback( uint32_t num, uint8_t digits_cnt, l3d_rtnl_t x, l3d_rtnl_t y, l3d_colour_t colour );
void lib3d_putInt32Callback( int32_t num, uint8_t digits_cnt, l3d_rtnl_t x, l3d_rtnl_t y, l3d_colour_t colour );
void lib3d_putRationalCallback( l3d_rtnl_t num, l3d_rtnl_t x, l3d_rtnl_t y, l3d_colour_t colour);



// 
// Debug:
// 

#define L3D_DEBUG 3
// Usage:
//  L3D_DEBUG_PRINT( "Setup complete %d\n", (int) 123 );

// #if defined(L3D_DEBUG) && L3D_DEBUG > 0
//  #define L3D_DEBUG_PRINT(fmt, args...) fprintf(stderr, "L3D_DEBUG: %s:%d:%s(): " fmt, \
//     __FILE__, __LINE__, __func__, ##args)
// #else
//  #define L3D_DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
// #endif

#endif	// _L3D_UTIL_H_