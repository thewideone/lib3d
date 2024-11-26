#include "../Inc/util.h"

void __attribute__((weak)) engine3D_putText( char* str, rtnl_t x, rtnl_t y, int size
#ifndef COLOUR_MONOCHROME
            , colour_t colour
#endif
            ){
}

void __attribute__((weak)) engine3D_drawLine( rtnl_t x0, rtnl_t y0, rtnl_t x1, rtnl_t y1
#ifndef COLOUR_MONOCHROME
            , colour_t colour
#endif
            ){
}