// 
// Main header of the 3D graphics engine; to be included by superproject.
// 

#ifndef _LIB3D_CORE_H_
#define _LIB3D_CORE_H_

#include "config3d.h"
#include "util.h"

void testFunc(void);

// 
// Draw a line on the screen.
// Used by lib3d_drawMesh().
// 
extern void lib3d_drawLineCallback( rtnl_t x0, rtnl_t y0, rtnl_t x1, rtnl_t y1, colour_t colour);

// 
// Just for debug.
// Should not be used as regular text-drawing function,
// because that one should be provided by a display library used.
// 
extern void lib3d_putTextCallback( char* str, rtnl_t x, rtnl_t y, colour_t colour);

// These are to be removed from here. They are based on putTextCallback and used only by this library.
// Or maybe they should be left to allow for optional override?
extern void lib3d_putUInt32Callback( uint32_t num, uint8_t digits_cnt, rtnl_t x, rtnl_t y, colour_t colour );
extern void lib3d_putRationalCallback( rtnl_t num, rtnl_t x, rtnl_t y, colour_t colour);

#endif // _LIB3D_CORE_H_