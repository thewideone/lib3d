#include "../Inc/lib3d_util.h"

#include <stdlib.h>	// for sprintf()

void __attribute__((weak)) l3d_drawLineCallback( int32_t x0, int32_t y0, int32_t x1, int32_t y1, l3d_colour_t colour ){
	/* NOTE: This function Should not be modified, when the callback is needed,
       l3d_drawLineCallback could be implemented in a user file
  	*/
}

void __attribute__((weak)) l3d_putTextCallback( int32_t x, int32_t y, char* str, l3d_colour_t colour ){
	/* NOTE: This function Should not be modified, when the callback is needed,
       l3d_putTextCallback could be implemented in a user file
  	*/
}

void __attribute__((weak)) l3d_putUInt32Callback( int32_t x, int32_t y, uint32_t num, uint8_t digits_cnt, l3d_colour_t colour ){
	/* NOTE: This function Should not be modified, when the callback is needed,
       l3d_putTextCallback could be implemented in a user file
  	*/

	if( digits_cnt ) {
		char buf[digits_cnt];
		for( uint8_t i=0; i<digits_cnt; i++ )	// is this needed?
			buf[i] = 0;
		snprintf(buf, digits_cnt, "%d", num);
		l3d_putTextCallback( x, y, buf, colour );
	}
	else {
		l3d_putTextCallback( x, y, (char*)"0\0", colour );
	}
}

void __attribute__((weak)) l3d_putInt32Callback( int32_t x, int32_t y, int32_t num, uint8_t digits_cnt, l3d_colour_t colour ){
	/* NOTE: This function Should not be modified, when the callback is needed,
       l3d_putTextCallback could be implemented in a user file
  	*/

	if( digits_cnt ) {
		char buf[digits_cnt];
		for( uint8_t i=0; i<digits_cnt; i++ )	// is this needed?
			buf[i] = 0;
		snprintf(buf, digits_cnt, "%d", num);
		l3d_putTextCallback( x, y, buf, colour );
	}
	else {
		l3d_putTextCallback( x, y, (char*)"0\0", colour );
	}
}
