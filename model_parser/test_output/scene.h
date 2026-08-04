#ifndef _SCENE_H_
#define _SCENE_H_

// 
// Generated for lib3d by scene descriptor generator by Szymon Kajda.
// Scene name: scene
// 

#include "lib3d_scene.h"

#define SCENE_OBJ_CUBE4D_NO_FACES_INSTANCE_COUNT 1

// Object instances ID's
#define SCENE_OBJ_CUBE4D_NO_FACES_I0_ID 0

// Number of different meshes in the scene
#define SCENE_MESH_COUNT 1
// Total number of objects in the scene (different meshes * their no. of instances)
#define SCENE_OBJ_COUNT 1
// Total number of cameras in the scene
#define SCENE_CAM_COUNT 1

extern l3d_scene_t scene;

l3d_err_t scene_init(void);

#endif // _SCENE_H_
