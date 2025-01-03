// 
// Main header of the 3D graphics engine; to be included by superproject.
// 

#ifndef _L3D_CORE_H_
#define _L3D_CORE_H_

#include "lib3d_config.h"
#include "lib3d_util.h"
#include "lib3d_scene.h"

// void testFunc(void);

// setupProjectionMatrix()
// computeViewMatrix()
// drawAllObjects(): requires drawMesh in scene.c
//					or just draw the whole edge array
// processObject()

// 
// Draw a line on the screen.
// Used by lib3d_drawMesh().
// 
extern void l3d_drawLineCallback( int32_t x0, int32_t y0, int32_t x1, int32_t y1, l3d_colour_t colour);

// 
// Just for debug.
// Should not be used as regular text-drawing function,
// because that one should be provided by a display library used.
// 
extern void l3d_putTextCallback( int32_t x, int32_t y, char* str, l3d_colour_t colour);

// These are to be removed from here. They are based on putTextCallback and used only by this library.
// Or maybe they should be left to allow for optional override?
extern void l3d_putUInt32Callback( int32_t x, int32_t y, uint32_t num, uint8_t digits_cnt, l3d_colour_t colour );
extern void l3d_putInt32Callback( int32_t x, int32_t y, int32_t num, uint8_t digits_cnt, l3d_colour_t colour );
// extern void l3d_putRationalCallback( int32_t x, int32_t y, l3d_rtnl_t num, l3d_colour_t colour);

#endif // _L3D_CORE_H_