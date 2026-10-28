#ifndef _L3D_ANIM_H_
#define _L3D_ANIM_H_

/* 
 * Animation module for lib3d.
 * This module enables user to create simple animations
 * using basic object transforms and modifying values
 * of particular structure properties over time.
 * 
 * An example of a description of a simple
 * animation is shown in lib3d_anim.c
*/

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_scene.h"	// for l3d_obj_type_t

// For animation state machine
enum l3d_anim_state
{
	L3D_ANIM_STATE_UNINITIALISED,
	L3D_ANIM_STATE_INITIALISED,
	L3D_ANIM_STATE_READY,
	L3D_ANIM_STATE_IN_PROGRESS,
	L3D_ANIM_STATE_PAUSED,
	L3D_ANIM_STATE_FINISHED,
};

// All operations possible to be animated
// (mostly from lib3d_transform.h and
// from particular struct members).
typedef enum
{
	// Scale (may be implemented in the future)
	// L3D_ANIM_PROP_SCALE_X,
	// L3D_ANIM_PROP_SCALE_Y,
	// L3D_ANIM_PROP_SCALE_Z,
	// Position
	L3D_ANIM_PROP_POS_GLOBAL,
	L3D_ANIM_PROP_POS_GLOBAL_X,
	L3D_ANIM_PROP_POS_GLOBAL_Y,
	L3D_ANIM_PROP_POS_GLOBAL_Z,
	L3D_ANIM_PROP_POS_LOCAL,
	L3D_ANIM_PROP_POS_LOCAL_X,
	L3D_ANIM_PROP_POS_LOCAL_Y,
	L3D_ANIM_PROP_POS_LOCAL_Z,
	// Rotate by quaternion
	// Noone's gonna say "rotate it by quaternion xyz"
	// aka hard to implement & won't be used by sane user
	// L3D_ANIM_PROP_ROT_GLOBAL_Q_ORIGIN,
	// L3D_ANIM_PROP_ROT_GLOBAL_Q_PIVOT,
	// L3D_ANIM_PROP_ROT_GLOBAL_Q,
	// Rotate about axis-angle
	L3D_ANIM_PROP_ROT_AXIS_ANGLE_GLOBAL_ORIGIN,	// always relative
	// L3D_ANIM_PROP_ROT_AXIS_ANGLE_GLOBAL_PIVOT,	// always relative
	L3D_ANIM_PROP_ROT_GLOBAL_PIVOT_GLOBAL_AXIS,	// always relative
	L3D_ANIM_PROP_ROT_GLOBAL_PIVOT_LOCAL_AXIS,	// always relative
	L3D_ANIM_PROP_ROT_LOCAL_PIVOT_GLOBAL_AXIS,	// always relative
	L3D_ANIM_PROP_ROT_LOCAL_PIVOT_LOCAL_AXIS,	// always relative
	L3D_ANIM_PROP_ROT_AXIS_ANGLE_GLOBAL,		// always relative
	L3D_ANIM_PROP_ROT_AXIS_ANGLE_LOCAL,			// always relative
	// Rotate about global axes
	L3D_ANIM_PROP_ROT_GLOBAL_X,
	L3D_ANIM_PROP_ROT_GLOBAL_Y,
	L3D_ANIM_PROP_ROT_GLOBAL_Z,
	// Rotate about object's local axes
	L3D_ANIM_PROP_ROT_LOCAL_X,
	L3D_ANIM_PROP_ROT_LOCAL_Y,
	L3D_ANIM_PROP_ROT_LOCAL_Z,
	// 3D object
	// L3D_ANIM_PROP_WIREFRAME_COLOUR,
	// Camera
	L3D_ANIM_PROP_FOV,
	L3D_ANIM_PROP_NEAR_PLANE,
	L3D_ANIM_PROP_FAR_PLANE,
} l3d_anim_property_t;

typedef enum
{
	L3D_INTERPOLATION_LINEAR,
	L3D_INTERPOLATION_STEP,
} l3d_interpolation_mode_t;

typedef enum
{
	L3D_EASING_LINEAR,
	L3D_EASING_IN,
	L3D_EASING_OUT,
	L3D_EASING_IN_OUT
} l3d_easing_mode_t;

typedef union
{
	l3d_vec4_t v4;	// when wanting to achieve some final position or sth
	l3d_quat_t q;	// when wanting to achieve some final orientation in quaternion format
	l3d_rot_t r;	// when wanting to achieve some final orientation in Euler angles
	l3d_rtnl_t val;	// value of the property at this keyframe (in rational format)
} l3d_prop_value_t;

typedef struct
{
	// Settings
	// bool is_value_relative;	// whether the value of the property at this keyframe is relative to the previous keyframe or absolute
	l3d_interpolation_mode_t interpolation;	// type of interpolation to use between keyframes
	l3d_easing_mode_t easing_mode;			// type of easing to use for the animation

	// Required values
	int32_t t;					// position of the keyframe on timeline in ticks
	const l3d_prop_value_t pv;	// property value
} l3d_keyframe_t;

// TODO: sort struct members in size descending order
typedef struct
{
	const l3d_anim_property_t property;

	const l3d_keyframe_t *keyframes;
	const uint16_t keyframe_count;

	// instead of for each keyframe
	const bool relative;	// whether the keyframe values are relative to the previous property value or absolute

	// Optional arguments
	l3d_vec4_t axis;	// for rotation, axis of rotation
	l3d_vec4_t pivot;	// for rotation, pivot point of rotation

	// 
	// Private
	// 

	// If action is relative to the previous property value,
	// initial property value needs to be stored.
	l3d_prop_value_t initial_pv;	// initial property value
	l3d_vec4_t current_axis;		// used in rotation about pivot
	l3d_vec4_t current_pivot;		// used in rotation about pivot
	l3d_rtnl_t prev_angle;			// for incremental rotation

} l3d_anim_action_t;

// Runtime playback information
typedef struct
{
	int16_t *processed_kf_indices;	// indices of already processed keyframes for each action (array of size action_count)
	uint16_t current_tick_no;		// current tick/frame number of the animation
	enum l3d_anim_state state;
} l3d_anim_playback_t;

typedef struct
{
	l3d_anim_playback_t playback;

	// See https://stackoverflow.com/questions/30929024/is-there-a-way-to-restrict-a-pointer-from-being-modified-by-specific-functions#comment49894419_30929165
	l3d_anim_action_t* const actions;
	// l3d_anim_action_t *actions;	// const?
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
// Trigger start of animation.
// It will be updated at next call of l3d_anim_update().
// 
l3d_err_t l3d_anim_start(
	l3d_anim_t *anim
);

// 
// Update (advance) the animation state
// based on the current tick number.
// 
l3d_err_t l3d_anim_update(
	l3d_scene_t *scene,
	l3d_anim_t *anim,
	uint8_t ticks_elapsed);

// 
// Pause given animation.
// Calling l3d_anim_update() will have no effect.
// 
l3d_err_t l3d_anim_pause(
	l3d_anim_t *anim
);

// 
// Resume given animation.
// Calling l3d_anim_update() will have effect.
// 
l3d_err_t l3d_anim_resume(
	l3d_anim_t *anim
);

#endif /* _L3D_ANIM_H_ */