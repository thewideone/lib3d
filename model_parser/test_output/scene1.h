#ifndef _SCENE1_H_
#define _SCENE1_H_

// 
// Generated for lib3d by scene descriptor generator by Szymon Kajda.
// Scene name: scene1
// Fixed point type: int32_t
// Fixed point binary digits: 16
// 

#include "lib3d_scene.h"

#define SCENE1_OBJ_CUBE_TRI_INSTANCE_COUNT 1
#define SCENE1_OBJ_PYRAMID_TRI_INSTANCE_COUNT 1

// Object instances ID's
#define SCENE1_OBJ_CUBE_TRI_I0_ID 0
#define SCENE1_OBJ_PYRAMID_TRI_I0_ID 1

#define SCENE1_OBJ_COUNT 2
#define SCENE1_CAM_COUNT 2

extern l3d_scene_t scene1;

l3d_err_t scene1_init(void);

#endif // _SCENE1_H_
