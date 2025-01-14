#ifndef _L3D_CONFIG_H_
#define _L3D_CONFIG_H_

// 
// Main configuration header of the 3D graphics library
// 

// 
// To make this library universal,
// this file and lib3d_core.h contain all dependencies
// on external features like drawing a line on a screen
// or camera control, so that the library can be used on
// any hardware.
// 

#include <inttypes.h>   // for int32_t etc.
#include <stddef.h>     // for NULL definition


// 
// Features configuration:
// 

#define L3D_CAMERA_MOVABLE				// Camera control defined below in this file
// #define USE_FILLED_MESHES
// #define USE_LOADING_FROM_OBJ
#define L3D_RENDER_VISIBLE_ONLY		// Render only visible edges / faces
// #define REMOVE_HIDDEN_LINES
// #define DRAW_CONTOUR_ONLY	// Draw only outlines of meshes



// 
// Display:
// 
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 400
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
	typedef uint32_t l3d_colour_t;
    #define L3D_COLOUR_WHITE 0xFFFFFFFFUL
	
#endif  // COLOUR_MONOCHROME



// 
// Math:
// 
#define L3D_USE_FIXED_POINT_ARITHMETIC

#include <stdbool.h> // c23 has some cool features - take a look

// Improper typedef (like unsigned int) causes a dramatic crash
// #ifndef _SIZE_T_DEFINED 
// #define _SIZE_T_DEFINED
// typedef unsigned long long size_t;
// #endif

// Fixed point arithmetic:
// Functions taken from javidx9's
// "Back To Basics! Fixed Point Numbers in C++" video

// Floating point number type
typedef float l3d_flp_t;
// Fixed point number type
typedef int32_t l3d_fxp_t;
// Twice-wide fixed point number type
typedef int64_t l3d_fxp2_t;
// Number of binary digits after the decimal place
#define L3D_FP_DP 16

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
// Rational number type (fxp_t for fixed point representation)
typedef l3d_fxp_t l3d_rtnl_t;
#else
// Rational number type (flp_t for floating point representation)
typedef l3d_flp_t l3d_rtnl_t;
#endif



// 
// Scene:
// 
#define L3D_SCENE_OBJ_CAP 10
#define L3D_SCENE_MODEL_VERTS_CAP 1024
#define L3D_SCENE_VERTS_CAP 2048
#define L3D_SCENE_FACES_CAP 2048
#define L3D_SCENE_EDGES_CAP 2048


// 
// Camera
// 
#ifdef L3D_CAMERA_MOVABLE
// Camera control functions.
// Sensitive only to constant key press.
// sf* - functions of SFML library
// #define L3D_CAMERA_MOVE_PRESSED_UP 0    //sfKeyboard_isKeyPressed(sfKeySpace)
// #define L3D_CAMERA_MOVE_PRESSED_DOWN 0  //sfKeyboard_isKeyPressed(sfKeyLShift) || sfKeyboard_isKeyPressed(sfKeyRShift)
// #define L3D_CAMERA_MOVE_PRESSED_LEFT 0  //sfKeyboard_isKeyPressed(sfKeyA)
// #define L3D_CAMERA_MOVE_PRESSED_RIGHT 0  //sfKeyboard_isKeyPressed(sfKeyD)
// #define L3D_CAMERA_MOVE_PRESSED_FORWARD 0    //sfKeyboard_isKeyPressed(sfKeyW)
// #define L3D_CAMERA_MOVE_PRESSED_BACKWARD 0  //sfKeyboard_isKeyPressed(sfKeyS)

// #define L3D_CAMERA_LOOK_PRESSED_UP 0    //sfKeyboard_isKeyPressed(sfKeyUp)
// #define L3D_CAMERA_LOOK_PRESSED_DOWN 0  //sfKeyboard_isKeyPressed(sfKeyDown)
// #define L3D_CAMERA_LOOK_PRESSED_LEFT 0  //sfKeyboard_isKeyPressed(sfKeyLeft)
// #define L3D_CAMERA_LOOK_PRESSED_RIGHT 0 //sfKeyboard_isKeyPressed(sfKeyRight)
// #define L3D_CAMERA_LOOK_PRESSED_ROLL_LEFT 0 //sfKeyboard_isKeyPressed(sfKeyComma)
// #define L3D_CAMERA_LOOK_PRESSED_ROLL_RIGHT 0    //sfKeyboard_isKeyPressed(sfKeyPeriod)

#endif // L3D_CAMERA_MOVABLE

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

typedef uint8_t l3d_err_t;
// Error codes:
#define L3D_OK                        0x00
// #define L3D_ERROR_NO_MEMORY           0x01
// #define L3D_ERROR_ALREADY_ASSIGNED    0x02
#define L3D_WRONG_PARAM               0x03

// Add L3D_ERROR_CHECK macro like in the ESP IDF?
// https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/error-handling.html


#endif	// _L3D_CONFIG_H_