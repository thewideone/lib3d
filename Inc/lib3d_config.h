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

#define L3D_EDGE_FLAGS_SINGLE_BYTE          // PackEdgeFlags = True

#ifdef L3D_EDGE_FLAGS_SINGLE_BYTE
#define L3D_EDGE_FLAG_VISIBILITY_BIT    2   // EdgeVisibilityFlagBitPos = 2
#define L3D_EDGE_FLAG_BOUNDARY_BIT      1   // EdgeBoundaryFlagBitPos = 1
#define L3D_EDGE_FLAG_SILHOUETTE_BIT    0   // EdgeSilhouetteFlagBitPos = 0
#endif  // L3D_EDGE_FLAGS_SINGLE_BYTE

// 
// Features configuration:
// 

#define L3D_CAMERA_MOVABLE				// Camera control defined below in this file
// #define USE_FILLED_MESHES
// #define USE_LOADING_FROM_OBJ
#define L3D_RENDER_VISIBLE_ONLY		// Render only visible edges / faces
// #define REMOVE_HIDDEN_LINES
// #define DRAW_CONTOUR_ONLY	// Draw only outlines of meshes
// #define L3D_USE_SCREEN_CLIPPING  // may be implemented in the future, but it's not a priority
#define L3D_DRAW_INNER_EDGES

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
    #else
	// Colour structure:
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
	// 	};
	// #endif
	// } l3d_colour_t;
	typedef uint32_t l3d_colour_t;

    // The following are taken from STM32 LCD Utility color definitions (stm32_lcd.h)
    #define L3D_COLOUR_BLUE          0xFF0000FFUL
    #define L3D_COLOUR_GREEN         0xFF00FF00UL
    #define L3D_COLOUR_RED           0xFFFF0000UL
    #define L3D_COLOUR_CYAN          0xFF00FFFFUL
    #define L3D_COLOUR_MAGENTA       0xFFFF00FFUL
    #define L3D_COLOUR_YELLOW        0xFFFFFF00UL
    #define L3D_COLOUR_LIGHTBLUE     0xFF8080FFUL
    #define L3D_COLOUR_LIGHTGREEN    0xFF80FF80UL
    #define L3D_COLOUR_LIGHTRED      0xFFFF8080UL
    #define L3D_COLOUR_LIGHTCYAN     0xFF80FFFFUL
    #define L3D_COLOUR_LIGHTMAGENTA  0xFFFF80FFUL
    #define L3D_COLOUR_LIGHTYELLOW   0xFFFFFF80UL
    #define L3D_COLOUR_DARKBLUE      0xFF000080UL
    #define L3D_COLOUR_DARKGREEN     0xFF008000UL
    #define L3D_COLOUR_DARKRED       0xFF800000UL
    #define L3D_COLOUR_DARKCYAN      0xFF008080UL
    #define L3D_COLOUR_DARKMAGENTA   0xFF800080UL
    #define L3D_COLOUR_DARKYELLOW    0xFF808000UL
    #define L3D_COLOUR_WHITE         0xFFFFFFFFUL
    #define L3D_COLOUR_LIGHTGRAY     0xFFD3D3D3UL
    #define L3D_COLOUR_GRAY          0xFF808080UL
    #define L3D_COLOUR_DARKGRAY      0xFF404040UL
    #define L3D_COLOUR_BLACK         0xFF000000UL
    #define L3D_COLOUR_BROWN         0xFFA52A2AUL
    #define L3D_COLOUR_ORANGE        0xFFFFA500UL
    #endif
	
#endif  // COLOUR_MONOCHROME



// 
// Math:
// 
#define L3D_USE_FIXED_POINT_ARITHMETIC

#include <stdbool.h> // c23 has some cool features - take a look

// Epsilon representing floating point tolerance in comparisons
#define L3D_EPSILON_FLP 0.001f //0.0000152587890625f // for 16 bits of fractional value
// #define L3D_EPSILON_FXP l3d_floatToFixed(L3D_EPSILON_FLP)
#define L3D_EPSILON_RTNL l3d_floatToRational(L3D_EPSILON_FLP)

#define L3D_BOUNDARY_EDGE_THRESHOLD l3d_floatToRational(0.8f)

// Improper typedef (like unsigned int) causes a dramatic crash
// #ifndef _SIZE_T_DEFINED 
// #define _SIZE_T_DEFINED
// typedef unsigned long long size_t;
// #endif

// TODO:
// change the name L3D_FP_DP to sth like L3D_FLP_DP
// move these typedefs into math.h
// VV





// 
// Scene:
// 
#define L3D_SCENE_OBJ_CAP 10
#define L3D_SCENE_MODEL_VERTS_CAP 1024
#define L3D_SCENE_VERTS_CAP 2048
#define L3D_SCENE_FACES_CAP 2048
#define L3D_SCENE_EDGES_CAP 2048

typedef enum l3d_dummy_obj_type_enum {
	L3D_OBJ_TYPE_CAMERA, L3D_OBJ_TYPE_OBJ3D
} l3d_obj_type_t;

// 
// Camera
// 

// Camera field of view in degrees
#define L3D_CAMERA_DEFAULT_FOV 90.0f
#define L3D_CAMERA_DEFAULT_NEAR_PLANE 0.1f
#define L3D_CAMERA_DEFAULT_FAR_PLANE 1000.0f

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

#define L3D_DEBUG 3

#define L3D_DEBUG_EDGES
// #define VERTEX_ID_DEBUG
// #define VERTEX_DOT_DEBUG

#define L3D_DEBUG_VISIBLE_EDGE_COLOUR L3D_COLOUR_GRAY
#define L3D_DEBUG_BOUNDARY_EDGE_COLOUR L3D_COLOUR_GREEN
#define L3D_DEBUG_SILHOUETTE_EDGE_COLOUR L3D_COLOUR_RED

typedef uint8_t l3d_err_t;

// Taken from:
// http://blog.man7.org/2012/10/how-much-do-builtinexpect-likely-and.html
// #define L3D_LIKELY(x)   __builtin_expect(!!(x), 1)
// #define L3D_UNLIKELY(x) __builtin_expect(!!(x), 0)

// ESP_ERROR_CHECK at:
// https://sourcevu.sysprogs.com/espressif/esp-idf/macros/ESP_ERROR_CHECK
// #define L3D_ERROR_CHECK(f) (l3d_err_t r = f; r == L3D_OK ? : return )

// In C23+?
// enum l3d_err : uint8_t {
//     bbb,
//     nnn
// }
// Error codes:
#define L3D_OK                        0x00
// #define L3D_ERROR_NO_MEMORY           0x01
// #define L3D_ERROR_ALREADY_ASSIGNED    0x02
#define L3D_WRONG_PARAM               0x03
#define L3D_DATA_EMPTY                0x04

// Add L3D_ERROR_CHECK macro like in the ESP IDF?
// https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/error-handling.html


#endif	// _L3D_CONFIG_H_
