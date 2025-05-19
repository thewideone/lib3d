#ifndef _SCENE_CUBE_H_
#define _SCENE_CUBE_H_

// 
// Generated for lib3d by scene descriptor generator by Szymon Kajda.
// Scene name: scene_cube
// Fixed point type: int32_t
// Fixed point binary digits: 16
// 

#include "lib3d_scene.h"

#define SCENE_CUBE_OBJ_CUBE_TRI_INSTANCE_COUNT 1

// Object instances ID's
#define SCENE_CUBE_OBJ_CUBE_TRI_I0_ID 0

// Number of different meshes in the scene
#define SCENE_CUBE_MESH_COUNT 1
// Total number of objects in the scene (different meshes * their no. of instances)
#define SCENE_CUBE_OBJ_COUNT 1
// Total number of cameras in the scene
#define SCENE_CUBE_CAM_COUNT 1

extern l3d_scene_t scene_cube;

l3d_err_t scene_cube_init(void);

#endif // _SCENE_CUBE_H_
