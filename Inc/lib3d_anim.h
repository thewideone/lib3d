#ifndef _L3D_ANIM_H_
#define _L3D_ANIM_H_

/* 
 * Animation module for lib3d.
 * This module enables user to create simple animations
 * using basic object transforms and modifying values
 * of particular structure properties over time.
 * 
 * Example of a header file describing a simple animation:
 * 
	#ifndef _ANIM_TEST_H_
	#define _ANIM_TEST_H_

	#include "lib3d_config.h"
	#include "lib3d_anim.h"

	static const l3d_keyframe_t anim_test_pos_global_X[] = {
		{ .pos = 0,  .value = 0  },
		{ .pos = 10, .value = 10 }
	};

	static const l3d_keyframe_t anim_test_pos_global_Y[] = {
		{ .pos = 0,  .value = 0  },
		{ .pos = 20, .value = 90 }
	};

	static const l3d_anim_action_t anim_test_actions[] = {
		{
			.property = L3D_ANIM_POS_GLOBAL_X,
			.keyframes = anim_test_pos_global_X,
			.keyframe_count = 2, // sizeof(anim_test_pos_global_X)/sizeof(l3d_keyframe_t),
		},
		{
			.property = L3D_ANIM_POS_GLOBAL_Y,
			.keyframes = anim_test_pos_global_Y,
			.keyframe_count = 2, // sizeof(anim_test_pos_global_Y)/sizeof(l3d_keyframe_t),
		}
	};

	// static?
	const l3d_anim_t anim_test = {
		.target_obj_type = L3D_OBJ_TYPE_OBJ3D,
		.target_obj_idx = 0,

		.actions = anim_test_actions,
		.action_count = 2,	// sizeof(anim_test_actions)/sizeof(l3d_anim_action_t),
	};

	#endif // _ANIM_TEST_H_
 * 
 * End of example code.
*/

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_scene.h"	// for l3d_obj_type_t

// All operations possible to be animated
// (mostly from lib3d_transform.h and
// from particular struct members).
typedef enum
{
	// L3D_ANIM_SCALE_X,
	// L3D_ANIM_SCALE_Y,
	// L3D_ANIM_SCALE_Z,
	// L3D_ANIM_POS_X,
	// L3D_ANIM_POS_Y,
	// L3D_ANIM_POS_Z,
	// L3D_ANIM_ROT_X,
	// L3D_ANIM_ROT_Y,
	// L3D_ANIM_ROT_Z,
	L3D_ANIM_POS_GLOBAL_X,
	L3D_ANIM_POS_GLOBAL_Y,
	L3D_ANIM_POS_GLOBAL_Z,
	L3D_ANIM_POS_LOCAL_X,
	L3D_ANIM_POS_LOCAL_Y,
	L3D_ANIM_POS_LOCAL_Z,
	// global rot...
	// local rot...
	// ...
	// L3D_ANIM_WIREFRAME_COLOUR,
} l3d_anim_property_t;

typedef enum
{
	L3D_INTERPOLATION_STEP,
	L3D_INTERPOLATION_LINEAR
} l3d_interpolation_t;

typedef enum
{
	L3D_EASING_LINEAR,
	L3D_EASING_IN,
	L3D_EASING_OUT,
	L3D_EASING_IN_OUT
} l3d_easing_t;

typedef struct
{
	// Settings
	bool is_value_relative;
	uint8_t interpolation;
	uint8_t easing;

	// Required values
	int32_t pos;
	l3d_rtnl_t value;

	// Optional arguments
	l3d_vec4_t axis_pivot;	// axis or pivot 
	l3d_vec4_t target_vec4;	// when wanting to achieve some final position or sth
	l3d_quat_t target_quat;	// when wanting to achieve some final rotation
} l3d_keyframe_t;

typedef struct
{
	l3d_anim_property_t property;

	const l3d_keyframe_t *keyframes;
	uint16_t keyframe_count;
} l3d_anim_action_t;

typedef struct
{
	l3d_obj_type_t target_obj_type;
	uint16_t target_obj_idx;

	const l3d_anim_action_t *actions;
	uint16_t action_count;
} l3d_anim_t;



#endif /* _L3D_ANIM_H_ */