#ifndef _LIB3D_CONFIG_H_
#define _LIB3D_CONFIG_H_

// 
// Main configuration header of the 3D graphics engine
// 

// 
// To make this engine universal,
// this and *.c files contain all dependencies
// on external features like drawing a line on a screen
// or camera control, so that the engine can be used on
// any hardware.
// 

#include <inttypes.h>   // for int32_t etc.
// #include "math3d.h"    // for colour_t
// 
// Hardware dependend stuff
// 
// See external_dependencies.h
// To avoid circular dependencies, hardware dependent
// functions and definitions were moved into
// external_dependencies.h and *.c files.

// For CSFML:
#define FRAMERATE 30



// 
// Engine features:
// 

#define USE_CAMERA				// Camera control defined below in this file
// #define USE_FILLED_MESHES
// #define USE_LOADING_FROM_OBJ
#define RENDER_VISIBLE_ONLY		// Render only visible edges / faces
// #define REMOVE_HIDDEN_LINES
// #define DRAW_CONTOUR_ONLY	// Draw only outlines of meshes



// 
// Display:
// 
#define SCREEN_WIDTH 240//390//256
#define SCREEN_HEIGHT 240//390//240
// #define COLOUR_MONOCHROME

#ifndef COLOUR_MONOCHROME
    // Number of bits per R/G/B channel
    #define COLOUR_DEPTH 8//2
    #if COLOUR_DEPTH <= 2
        #define COLOUR_SINGLE_BYTE
        #define COLOUR_WHITE   0xff
        #define COLOUR_BLACK   0x00
        #if COLOUR_DEPTH == 1
            //  _______________
            // |-|R|-|G|-|B|-|-|
            // 
            #define COLOUR_RED     0b01000000
            #define COLOUR_GREEN   0b00010000
            #define COLOUR_BLUE    0b00000100
        #else
            //  _______________
            // |R|R|G|G|B|B|-|-|
            // 
            #define COLOUR_RED     0b11000000
            #define COLOUR_GREEN   0b00110000
            #define COLOUR_BLUE    0b00001100
        #endif

        #define COLOUR_CYAN    (COLOUR_GREEN | COLOUR_BLUE)
        #define COLOUR_MAGENTA (COLOUR_RED   | COLOUR_BLUE)
        #define COLOUR_YELLOW  (COLOUR_RED   | COLOUR_GREEN)
    #endif

	// // Colour structure:
	// typedef struct {
	// #ifdef COLOUR_SINGLE_BYTE
	// 	uint8_t rgb;
	// #else
	// 	union {
	// 		struct {
	// 			uint8_t r;
	// 			uint8_t g;
	// 			uint8_t b;
	// 		} channels;
	// 		uint32_t value;
	// 	}
	// #endif
	// } colour_t;
	#define colour_t uint32_t
	
#endif  // COLOUR_MONOCHROME



// 
// Math:
// 
#define LIB3D_USE_FIXED_POINT_ARITHMETIC

#define true 1
#define false 0
#define bool int

// Improper typedef (like unsigned int) causes a dramatic crash
// #ifndef _SIZE_T_DEFINED 
// #define _SIZE_T_DEFINED
// typedef unsigned long long size_t;
// #endif

// Fixed point arithmetic:
// Functions taken from javidx9's
// "Back To Basics! Fixed Point Numbers in C++" video

// Floating point number type
#define flp_t float
// Fixed point number type
#define fxp_t int32_t
// Twice-wide fixed point number type
#define fxp2_t int64_t
// Number of binary digits after the decimal place
#define FP_DP 16

#ifdef LIB3D_USE_FIXED_POINT_ARITHMETIC
// Rational number type (fxp_t for fixed point representation)
#define rtnl_t fxp_t
// For debug:
#define STDO_RTNL STDO_INT32
#else
// Rational number type (flp_t for floating point representation)
#define rtnl_t flp_t
// For debug:
#define STDO_RTNL STDO_FLT
#endif



// 
// Mesh queue (mesh_3d_queue.*):
// 
#define MESH_QUEUE_FIXED_SIZE
#define MESH_QUEUE_CAPACITY 10



#ifdef USE_CAMERA
// Camera control functions.
// Sensitive only to constant key press.
// sf* - functions of SFML library
#define CAMERA_MOVE_PRESSED_UP sfKeyboard_isKeyPressed(sfKeySpace)
#define CAMERA_MOVE_PRESSED_DOWN sfKeyboard_isKeyPressed(sfKeyLShift) || sfKeyboard_isKeyPressed(sfKeyRShift)
#define CAMERA_MOVE_PRESSED_LEFT sfKeyboard_isKeyPressed(sfKeyA)
#define CAMERA_MOVE_PRESSED_RIGHT sfKeyboard_isKeyPressed(sfKeyD)
#define CAMERA_MOVE_PRESSED_FORWARD sfKeyboard_isKeyPressed(sfKeyW)
#define CAMERA_MOVE_PRESSED_BACKWARD sfKeyboard_isKeyPressed(sfKeyS)

#define CAMERA_LOOK_PRESSED_UP sfKeyboard_isKeyPressed(sfKeyUp)
#define CAMERA_LOOK_PRESSED_DOWN sfKeyboard_isKeyPressed(sfKeyDown)
#define CAMERA_LOOK_PRESSED_LEFT sfKeyboard_isKeyPressed(sfKeyLeft)
#define CAMERA_LOOK_PRESSED_RIGHT sfKeyboard_isKeyPressed(sfKeyRight)
#define CAMERA_LOOK_PRESSED_ROLL_LEFT sfKeyboard_isKeyPressed(sfKeyComma)
#define CAMERA_LOOK_PRESSED_ROLL_RIGHT sfKeyboard_isKeyPressed(sfKeyPeriod)

#endif // USE_CAMERA

// void engine3D_register_drawLine( void *function_ptr );
// void engine3D_register_putText( void *function_ptr );



// #ifdef USE_LOADING_FROM_OBJ
// #include <stdlib.h>      // for reading files
// #include <stdio.h>
// #include <errno.h>
// // Used only by getline() below
// ssize_t getdelim(char **linep, size_t *n, int delim, FILE *fp);
// ssize_t getline(char **linep, size_t *n, FILE *fp);
// #endif



// 
// Debug:
// 

// #define VERTEX_ID_DEBUG
// #define VERTEX_DOT_DEBUG



#endif	// _LIB3D_CONFIG_H_