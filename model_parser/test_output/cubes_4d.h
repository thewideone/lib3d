#ifndef _CUBES_4D_H_
#define _CUBES_4D_H_

// 
// Generated for lib3d by scene descriptor generator by Szymon Kajda.
// Scene name: cubes_4d
// 

#include "lib3d_scene.h"

#define CUBES_4D_OBJ_CUBE4D_NO_FACES_INSTANCE_COUNT 1
#define CUBES_4D_OBJ_CUBE4D_WITH_FACES_INSTANCE_COUNT 1

// Object instances ID's
#define CUBES_4D_OBJ_CUBE4D_NO_FACES_I0_ID 0
#define CUBES_4D_OBJ_CUBE4D_WITH_FACES_I0_ID 1

// Number of different meshes in the scene
#define CUBES_4D_MESH_COUNT 2
// Total number of objects in the scene (different meshes * their no. of instances)
#define CUBES_4D_OBJ_COUNT 2
// Total number of cameras in the scene
#define CUBES_4D_CAM_COUNT 1

extern l3d_scene_t cubes_4d;

l3d_err_t cubes_4d_init(void);

#endif // _CUBES_4D_H_
