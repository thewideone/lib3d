#ifndef _LIB3D_OBJ3D_QUEUE_H_
#define _LIB3D_OBJ3D_QUEUE_H_

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_obj3d.h"

typedef struct {
	obj3d_t* objects[LIB3D_OBJ_QUEUE_CAP];
	uint8_t current_idx;
	obj3d_t* current_obj;	// for optimization?
} obj3d_queue_t;


#endif	// _LIB3D_OBJ3D_QUEUE_H_