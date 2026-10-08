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
		{ .t = 0,  .value = L3D_FLOAT_TO_RATIONAL(0.0f)  },
		{ .t = 10, .value = L3D_FLOAT_TO_RATIONAL(1.0f) }
	};

	static const l3d_keyframe_t anim_test_pos_global_Y[] = {
		{ .t = 0,  .value = L3D_FLOAT_TO_RATIONAL(0.0f)  },
		{ .t = 20, .value = L3D_FLOAT_TO_RATIONAL(2.0f) }
	};

	static l3d_anim_action_t anim_test_actions[] = {
		{
			.property = L3D_ANIM_PROP_POS_GLOBAL_X,
			.keyframes = anim_test_pos_global_X,
			.keyframe_count = sizeof(anim_test_pos_global_X)/sizeof(l3d_keyframe_t),
			.relative = true,
		},
		{
			.property = L3D_ANIM_PROP_POS_GLOBAL_Y,
			.keyframes = anim_test_pos_global_Y,
			.keyframe_count = sizeof(anim_test_pos_global_Y)/sizeof(l3d_keyframe_t),
			.relative = false,
		}
	};

	static int16_t anim_test_processed_kf_indices[sizeof(anim_test_actions)/sizeof(l3d_anim_action_t)] = {-1};

	l3d_anim_t anim_test = {
		.target_obj_type = L3D_OBJ_TYPE_OBJ3D,
		.target_obj_idx = 0,
		.repeat = true,

		.actions = anim_test_actions,
		.action_count = sizeof(anim_test_actions)/sizeof(l3d_anim_action_t),

		.state = {
			.current_tick_no = 0,
			.processed_kf_indices = anim_test_processed_kf_indices,
			.in_progress = false,
			.is_finished = false,
		},

	};

	#endif // _ANIM_TEST_H_ 
 * 
 * End of example code.
*/

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_scene.h"	// for l3d_obj_type_t

// typedef enum
// {
// 	L3D_ANIM_ACTION_FLAG_IN_PROGRESS	= 0x00,	// action is currently in progress
// 	L3D_ANIM_ACTION_FLAG_FINISHED 		= 0x01,	// action has finished (reached the last keyframe)
// } l3d_anim_action_status_t;

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
	L3D_ANIM_PROP_POS_GLOBAL_X,
	L3D_ANIM_PROP_POS_GLOBAL_Y,
	L3D_ANIM_PROP_POS_GLOBAL_Z,
	L3D_ANIM_PROP_POS_LOCAL_X,
	L3D_ANIM_PROP_POS_LOCAL_Y,
	L3D_ANIM_PROP_POS_LOCAL_Z,
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
	// bool is_value_relative;	// whether the value of the property at this keyframe is relative to the previous keyframe or absolute
	uint8_t interpolation;	// type of interpolation to use between keyframes
	uint8_t easing;			// type of easing to use for the animation

	// Required values
	int32_t t;				// position of the keyframe on timeline in ticks
	union {
		l3d_vec4_t target_vec4;	// when wanting to achieve some final position or sth
		l3d_quat_t target_quat;	// when wanting to achieve some final orientation in quaternion format
		l3d_rot_t target_rot;	// when wanting to achieve some final orientation in Euler angles
		l3d_rtnl_t value;		// value of the property at this keyframe (in rational format)
	};

	// Optional arguments
	union {
		l3d_vec4_t axis;	// for rotation, axis of rotation
		l3d_vec4_t pivot;	// for rotation, pivot point of rotation
	};
} l3d_keyframe_t;

typedef struct
{
	const l3d_anim_property_t property;

	const l3d_keyframe_t *keyframes;
	const uint16_t keyframe_count;

	// instead of for each keyframe
	const bool relative;	// whether the keyframe values are relative to the previous property value or absolute

	// If action is relative to the previous property value,
	// initial property value needs to be stored.
	union {
		l3d_vec4_t initial_vec4;	// when wanting to achieve some final position or sth
		l3d_quat_t initial_quat;	// when wanting to achieve some final orientation in quaternion format
		l3d_rot_t initial_rot;		// when wanting to achieve some final orientation in Euler angles
		l3d_rtnl_t initial_value;	// initial value of the property (in rational format)
	};

} l3d_anim_action_t;

typedef struct
{
	// const l3d_anim_t *anim;
	// uint16_t current_action_idx;
	// uint16_t current_keyframe_idx;
	int16_t *processed_kf_indices;	// indices of already processed keyframes for each action (array of size action_count)
	uint16_t current_tick_no;		// current tick/frame number of the animation
	// l3d_rtnl_t current_value;

	bool in_progress;	// if true, animation is currently in progress
	bool is_finished;	// if true, animation has finished (reached the last keyframe)
} l3d_anim_state_t;

typedef struct
{
	l3d_anim_state_t state;

	l3d_anim_action_t *actions;	// const?
	const uint16_t action_count;
	uint8_t *action_flags;	// array of flags indicating whether each action is finished
	uint16_t last_keyframe_pos;	// const?; position of the last keyframe in ticks

	l3d_obj_type_t target_obj_type;
	uint16_t target_obj_idx;

	// int32_t repeat_offset;
	bool repeat;			// if true, animation will loop over the keyframes
} l3d_anim_t;

// 
// Public functions
// 

// 
// Initialize the animation state for a given animation.
// 
l3d_err_t l3d_anim_init(
	l3d_anim_t *anim,
	const l3d_scene_t *scene);

// 
// Reset the animation to its initial state.
// 
l3d_err_t l3d_anim_reset(
	l3d_anim_t *anim,
	const l3d_scene_t *scene);

// 
// Update (advance) the animation state based on the current tick number.
// 
l3d_err_t l3d_anim_update(
	l3d_scene_t *scene,
	l3d_anim_t *anim,
	uint8_t ticks_elapsed);

#endif /* _L3D_ANIM_H_ */