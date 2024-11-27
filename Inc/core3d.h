// 
// Main header of the 3D graphics engine; to be included by superproject.
// 

#ifndef _GRAPHICS_ENGINE_3D_CORE_H_
#define _GRAPHICS_ENGINE_3D_CORE_H_

#include "config3d.h"
#include "util.h"

void testFunc(void);

// 
// Just for debug.
// Should not be used as regular text-drawing function,
// because that one should be provided by a display library used.
// 
extern void engine3D_putTextCallback( char* str, rtnl_t x, rtnl_t y, int size
#ifndef COLOUR_MONOCHROME
                      , colour_t colour
#endif
                      );

// 
// Draw a line on the screen.
// Used by engine3D_drawMesh().
// 
extern void engine3D_drawLineCallback( rtnl_t x0, rtnl_t y0, rtnl_t x1, rtnl_t y1
#ifndef COLOUR_MONOCHROME
                       , colour_t colour
#endif
                      );

#endif