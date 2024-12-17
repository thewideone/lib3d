#ifndef _LIB3D_SCENE_H_
#define _LIB3D_SCENE_H_

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_obj3d.h"

typedef struct {
	obj3d_t* objects[LIB3D_SCENE_OBJ_CAP];
} scene_t;


#endif	// _LIB3D_SCENE_H_