#include "../Inc/util.h"

#include <stdlib.h>	// for itoa()

void __attribute__((weak)) lib3d_drawLineCallback( rtnl_t x0, rtnl_t y0, rtnl_t x1, rtnl_t y1, colour_t colour ){
	/* NOTE: This function Should not be modified, when the callback is needed,
       the lib3d_drawLineCallback could be implemented in a user file
  	*/
}

void __attribute__((weak)) lib3d_putTextCallback( char* str, rtnl_t x, rtnl_t y, colour_t colour ){
	/* NOTE: This function Should not be modified, when the callback is needed,
       the lib3d_putTextCallback could be implemented in a user file
  	*/
}

void __attribute__((weak)) lib3d_putUInt32Callback( uint32_t num, uint8_t digits_cnt, rtnl_t x, rtnl_t y, colour_t colour ){
	/* NOTE: This function Should not be modified, when the callback is needed,
       the lib3d_putTextCallback could be implemented in a user file
  	*/

	if( digits_cnt ) {
		char buf[digits_cnt];
		for( uint8_t i=0; i<digits_cnt; i++ )
			buf[i] = 0;
		itoa( num, buf, 10 );	// 10 for decimal base
		lib3d_putTextCallback( buf, x, y, colour );
	}
	else {
		lib3d_putTextCallback( (char*)"0\0", x, y, colour );
	}
}

