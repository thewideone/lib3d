#include "../Inc/util.h"

void __attribute__((weak)) engine3D_putTextCallback( char* str, rtnl_t x, rtnl_t y, int size
#ifndef COLOUR_MONOCHROME
            , colour_t colour
#endif
            ){
	/* NOTE: This function Should not be modified, when the callback is needed,
       the engine3D_putTextCallback could be implemented in a user file
  	*/
}

void __attribute__((weak)) engine3D_drawLineCallback( rtnl_t x0, rtnl_t y0, rtnl_t x1, rtnl_t y1
#ifndef COLOUR_MONOCHROME
            , colour_t colour
#endif
            ){
	/* NOTE: This function Should not be modified, when the callback is needed,
       the engine3D_drawLineCallback could be implemented in a user file
  	*/
}