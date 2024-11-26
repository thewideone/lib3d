#ifndef _GRAPHICS_ENGINE_3D_UTIL_H_
#define _GRAPHICS_ENGINE_3D_UTIL_H_

// 
// This file contains stuff from
// external_dependencies.h in the CSFML version
// of this engine.
// 

#include "config3d.h"

// 
// Just for debug.
// Should not be used as regular text-drawing function,
// because that one should be provided by a display library used.
// 
void engine3D_putText( char* str, rtnl_t x, rtnl_t y, int size
#ifndef COLOUR_MONOCHROME
                      , colour_t colour
#endif
                      );

// 
// Draw a line on the screen.
// Used by engine3D_drawMesh().
// 
void engine3D_drawLine( rtnl_t x0, rtnl_t y0, rtnl_t x1, rtnl_t y1
#ifndef COLOUR_MONOCHROME
                       , colour_t colour
#endif
                      );

#endif	// _GRAPHICS_ENGINE_3D_UTIL_H_