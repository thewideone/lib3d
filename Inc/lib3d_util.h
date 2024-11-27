#ifndef _LIB3D_UTIL_H_
#define _LIB3D_UTIL_H_

// 
// This file contains stuff from
// external_dependencies.h in the CSFML version
// of this engine.
// 

#include "lib3d_config.h"

// 
// Draw a line on the screen.
// Used by lib3d_drawMesh().
// 
void lib3d_drawLineCallback( rtnl_t x0, rtnl_t y0, rtnl_t x1, rtnl_t y1, colour_t colour);

// 
// Just for debug.
// Should not be used as regular text-drawing function,
// because that one should be provided by a display library used.
// 
void lib3d_putTextCallback( char* str, rtnl_t x, rtnl_t y, colour_t colour);
void lib3d_putUInt32Callback( uint32_t num, uint8_t digits_cnt, rtnl_t x, rtnl_t y, colour_t colour );
void lib3d_putRationalCallback( rtnl_t num, rtnl_t x, rtnl_t y, colour_t colour);



// 
// Debug:
// 

// Simple default text output function (for string, char, int, float, etc)
// For places where non-debug text output is the purpose of a function
// Usage:
//  STDO_STR( "yo mama obama" );
//  STDO_CHR( 'F' );
//  STDO_INT( 123 );
//  STDO_FLT( 12.34 );
// #define STDO_STR(x) printf(x)
// #define STDO_CHR(x) printf("%c", x)
// #define STDO_INT(x) printf("%d", x)
// #define STDO_FLT(x) printf("%f", x)

// #define STDO_INT8   STDO_INT
// #define STDO_UINT8  STDO_INT
// #define STDO_UINT16 STDO_INT
// #define STDO_INT16  STDO_INT
// #define STDO_UINT32 STDO_INT
// #define STDO_INT32  STDO_INT
// // For size_t:
// #define STDO_UINT64(x) printf("%lld", x)
// #define STDO_INT64  STDO_INT

// #define STDO_SIZET STDO_UINT64

#define LIB3D_DEBUG 3
// Usage:
//  LIB3D_DEBUG_PRINT( "Setup complete %d\n", (int) 123 );

#if defined(LIB3D_DEBUG) && LIB3D_DEBUG > 0
 #define LIB3D_DEBUG_PRINT(fmt, args...) fprintf(stderr, "LIB3D_DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
 #define LIB3D_DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

#endif	// _LIB3D_UTIL_H_