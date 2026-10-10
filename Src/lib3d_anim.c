#include "lib3d_config.h"
#include "lib3d_anim.h"
#include "lib3d_util.h"
#include "lib3d_math.h"
#include "lib3d_scene.h"
#include "lib3d_transform.h"

// 
// Reset given animation in given scene
// 
l3d_err_t l3d_anim_reset(
	l3d_anim_t *anim,
	const l3d_scene_t *scene)
{
	if (anim == NULL)
		return L3D_WRONG_PARAM;

	anim->state.current_tick_no = 0;
	anim->state.in_progress = false;
	anim->state.is_finished = false;

	for (uint16_t action_idx = 0; action_idx < anim->action_count; action_idx++)
	{
		// Reset processed keyframe indices for each action
		anim->state.processed_kf_indices[action_idx] = -1;

		// If action is relative, save initial property value
		if (anim->actions[action_idx].relative)
		{
			L3D_DEBUG_PRINT("Saving initial value for action %d (property %d) for object type %d ID %d\n",
				action_idx, anim->actions[action_idx].property, anim->target_obj_type, anim->target_obj_idx);
			
			switch (anim->actions[action_idx].property)
			{
				case L3D_ANIM_PROP_POS_GLOBAL_X:
					anim->actions[action_idx].initial_value = l3d_scene_getObjectLocalPos(scene, anim->target_obj_type, anim->target_obj_idx).x;
					break;
				case L3D_ANIM_PROP_POS_GLOBAL_Y:
					anim->actions[action_idx].initial_value = l3d_scene_getObjectLocalPos(scene, anim->target_obj_type, anim->target_obj_idx).y;
					break;
				case L3D_ANIM_PROP_POS_GLOBAL_Z:
					anim->actions[action_idx].initial_value = l3d_scene_getObjectLocalPos(scene, anim->target_obj_type, anim->target_obj_idx).z;
					break;
				default:
					L3D_DEBUG_PRINT("Warning: Unknown property type %d for action %d. Initial value not saved.\n",
						anim->actions[action_idx].property, action_idx);
					break;
			}
		}
	}

	return L3D_OK;
}

// 
// Initialise given animation in given scene
// 
l3d_err_t l3d_anim_init(
	l3d_anim_t *anim,
	const l3d_scene_t *scene)
{
	if (anim == NULL || scene == NULL)
		return L3D_WRONG_PARAM;

	// Compute tick number of the last keyframe across all actions
	// to see when the animation should finish (if not repeating)
	uint16_t last_keyframe_pos = 0;
	for (uint16_t action_idx = 0; action_idx < anim->action_count; action_idx++)
	{
		if (anim->actions[action_idx].keyframe_count > 0)
		{
			uint16_t last_keyframe_idx = anim->actions[action_idx].keyframe_count - 1;
			uint16_t last_keyframe_tick_no = anim->actions[action_idx].keyframes[last_keyframe_idx].t;
			if (last_keyframe_tick_no > last_keyframe_pos)
				last_keyframe_pos = last_keyframe_tick_no;
		}
	}
	anim->last_keyframe_pos = last_keyframe_pos;

	return l3d_anim_reset(anim, scene);
}

// 
// Apply easing function to given parameter t.
// Currently only quadratic functions are used
// 
l3d_rtnl_t l3d_ease(
	l3d_rtnl_t t,
	const l3d_easing_mode_t mode)
{
	// Clamp parameter t to [0,1]
	if (t > L3D_RTNL_ONE)
		t = L3D_RTNL_ONE;
	else if (t < L3D_RTNL_ZERO)
		t = L3D_RTNL_ZERO;
	
    switch (mode)
    {
        case L3D_EASING_IN:
			// t^2
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
			return l3d_fixedMul(t, t);
#else
			return t*t;
#endif	// L3D_USE_FIXED_POINT_ARITHMETIC
			break;
        case L3D_EASING_OUT:
			// 1 - (1 - t)^2
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
			l3d_rtnl_t one_minus_t = L3D_RTNL_ONE - t;
			return L3D_RTNL_ONE - l3d_fixedMul(one_minus_t, one_minus_t);
#else
			return 1.0f - (1.0f - t)*(1.0f - t);
#endif	// L3D_USE_FIXED_POINT_ARITHMETIC
			break;
        case L3D_EASING_IN_OUT:
			// 2t^2			if t <  0.5f
			// 1-2(1-t)^2	if t >= 0.5f
            if (t < l3d_floatToRational(0.5f))
            {
                
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
				return l3d_fixedMul(l3d_floatToRational(2.0f),
									l3d_fixedMul(t, t));
#else
				return 2.0f * t*t;
#endif	// L3D_USE_FIXED_POINT_ARITHMETIC
            }
            else
            {
#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
				l3d_rtnl_t one_minus_t = L3D_RTNL_ONE - t;
                return L3D_RTNL_ONE - l3d_fixedMul(
					l3d_floatToRational(2.0f),
					l3d_fixedMul(one_minus_t, one_minus_t));
#else
				return 1.0f - 2.0f * (1.0f - t)*(1.0f - t);
#endif	// L3D_USE_FIXED_POINT_ARITHMETIC
            }
			break;
        case L3D_EASING_LINEAR:
        default:
            return t;
    }
}

// 
// Update given action property at given keyframe
// in given scene for given target object
// 
static l3d_err_t l3d_anim_update_prop(
	const l3d_anim_action_t *action,
	const l3d_keyframe_t *kf,
	l3d_scene_t *scene,
	const l3d_obj_type_t target_obj_type,
	const uint16_t target_obj_idx
)
{
	l3d_err_t ret;
	l3d_rtnl_t current_prop_value, delta_value;
	switch (action->property)
	{
		case L3D_ANIM_PROP_POS_GLOBAL_X:
			current_prop_value = l3d_scene_getObjectLocalPos(scene, target_obj_type, target_obj_idx).x;

			delta_value = kf->value - current_prop_value;
			
			if (action->relative)
			{
				delta_value += action->initial_value;
			}

			if (l3d_abs(delta_value) < L3D_EPSILON_RTNL )
				break;	// no need to update if the value is already close enough
			
			ret = l3d_moveGlobalX(scene, target_obj_type, target_obj_idx, delta_value);
			break;
		case L3D_ANIM_PROP_POS_GLOBAL_Y:
			current_prop_value = l3d_scene_getObjectLocalPos(scene, target_obj_type, target_obj_idx).y;

			delta_value = kf->value - current_prop_value;
			
			if (action->relative)
			{
				delta_value += action->initial_value;
			}

			if (l3d_abs(delta_value) < L3D_EPSILON_RTNL)
				break;	// no need to update if the value is already close enough
			
			ret = l3d_moveGlobalY(scene, target_obj_type, target_obj_idx, delta_value);
			break;
		default:
			return L3D_WRONG_PARAM;
	}

	if (l3d_abs(delta_value) < L3D_EPSILON_RTNL)
	{
		L3D_DEBUG_PRINT("init: %f, current: %f, kf->value: %f, delta: %f\n",
			l3d_rationalToFloat(action->initial_value),
			l3d_rationalToFloat(current_prop_value),
			l3d_rationalToFloat(kf->value),
			l3d_rationalToFloat(delta_value));

		L3D_DEBUG_PRINT("Updated property by value %f\n",
			l3d_rationalToFloat(delta_value));
	}

	return ret;
}

// 
// Update given action property for given keyframe
// with interpolation,
// in given scene for given target object.
// Modify interpolation factor if needed by easing
// 
static l3d_err_t l3d_anim_update_prop_intp(
	const l3d_anim_action_t *action,
	const l3d_keyframe_t *last_kf,
	const l3d_keyframe_t *next_kf,
	l3d_scene_t *scene,
	const l3d_obj_type_t target_obj_type,
	const uint16_t target_obj_idx,
	l3d_rtnl_t *interpolation_factor
)
{
	l3d_err_t ret;
	l3d_rtnl_t last_prop_value, current_prop_value, next_prop_value, interpolated_value, delta_value;
	switch (action->property)
	{
		case L3D_ANIM_PROP_POS_GLOBAL_X:
			last_prop_value = last_kf->value; //last_kf->target_vec4.x;	// or other
			current_prop_value = l3d_scene_getObjectLocalPos(scene, target_obj_type, target_obj_idx).x;
			
			next_prop_value = next_kf->value; //next_kf->target_vec4.x;	// or other

			if (action->relative)
			{
				current_prop_value -= action->initial_value;
			}

			// switch (next_kf->interpolation)...

			*interpolation_factor = l3d_ease(*interpolation_factor, next_kf->easing_mode);

			interpolated_value = l3d_lerp(last_prop_value, next_prop_value, *interpolation_factor);
			delta_value = interpolated_value - current_prop_value;

			ret = l3d_moveGlobalX(scene, target_obj_type, target_obj_idx, delta_value);
			break;
		case L3D_ANIM_PROP_POS_GLOBAL_Y:
			last_prop_value = last_kf->value; //last_kf->target_vec4.y;	// or other
			current_prop_value = l3d_scene_getObjectLocalPos(scene, target_obj_type, target_obj_idx).y;
			next_prop_value = next_kf->value; //next_kf->target_vec4.y;	// or other

			if (action->relative)
			{
				current_prop_value -= action->initial_value;
			}

			// switch (next_kf->interpolation)...
			
			*interpolation_factor = l3d_ease(*interpolation_factor, next_kf->easing_mode);

			interpolated_value = l3d_lerp(last_prop_value, next_prop_value, *interpolation_factor);
			delta_value = interpolated_value - current_prop_value;
			
			ret = l3d_moveGlobalY(scene, target_obj_type, target_obj_idx, delta_value);
			break;
		default:
			return L3D_WRONG_PARAM;
	}

	L3D_DEBUG_PRINT("current %f, next %f, interpolated %f (factor %f), delta %f\n",
		l3d_rationalToFloat(current_prop_value),
		l3d_rationalToFloat(next_prop_value),
		l3d_rationalToFloat(interpolated_value),
		l3d_rationalToFloat(*interpolation_factor),
		l3d_rationalToFloat(delta_value));
	
	return ret;
}

// 
// Update given animation in given scene
// by given number of animation ticks
// 
l3d_err_t l3d_anim_update(
	l3d_scene_t *scene,
	l3d_anim_t *anim,
	uint8_t ticks_elapsed)
{
	if (anim == NULL || scene == NULL)
		return L3D_WRONG_PARAM;
	
	if (anim->state.is_finished)
	{
		L3D_DEBUG_PRINT("Animation for object type %d ID %d is already finished. No update performed.\n",
			anim->target_obj_type, anim->target_obj_idx);
		return L3D_OK;
	}

	l3d_err_t ret = L3D_OK;
	int32_t new_tick_no = anim->state.current_tick_no + ticks_elapsed;

	if (!anim->state.in_progress)
	{
		anim->state.in_progress = true;
		anim->state.is_finished = false;
		new_tick_no = 0;					// start from the beginning
		L3D_DEBUG_PRINT("Animation for object type %d ID %d started.\n",
			anim->target_obj_type, anim->target_obj_idx);
	}

	L3D_DEBUG_PRINT("===== Tick: current %d, new %d =====\n",
		anim->state.current_tick_no, new_tick_no);

	// For each action
	for (uint8_t action_idx = 0; action_idx < anim->action_count; action_idx++)
	{
		// If no keyframes for this action, skip it
		if (anim->actions[action_idx].keyframe_count == 0)
		{
			L3D_DEBUG_PRINT("Animation for object type %d ID %d has action %d with no keyframes. Skipping.\n",
				anim->target_obj_idx, anim->target_obj_type, action_idx);
			continue;
		}

		L3D_DEBUG_PRINT("== Action %d (%s):\n",
			action_idx,
			anim->actions[action_idx].relative ? "relative" : "absolute");

		int16_t processed_kf_idx = anim->state.processed_kf_indices[action_idx];

		// For each unprocessed keyframe in the action up to the new tick number,
		// apply the keyframe value to the target object's property
		for (int16_t kf_idx = anim->state.processed_kf_indices[action_idx];
			kf_idx < anim->actions[action_idx].keyframe_count;
			kf_idx++)
		{
			// Begin with first keyframe if none has been processed yet
			if (kf_idx < 0) {
				kf_idx = 0;	
			}

			const l3d_keyframe_t *kf = &anim->actions[action_idx].keyframes[kf_idx];

			L3D_DEBUG_PRINT("Last processed keyframe index: %d\n",
				anim->state.processed_kf_indices[action_idx]);
			L3D_DEBUG_PRINT("Checking keyframe %d at tick %d...\n",
				kf_idx, kf->t);
			
			// If the keyframe has already been processed, skip it
			if (kf_idx <= anim->state.processed_kf_indices[action_idx])
			{
				L3D_DEBUG_PRINT("This keyframe has already been processed. Skipping.\n");
				continue;
			}

			// If the keyframe's tick is less than or equal to the new tick number
			if (kf->t <= new_tick_no)
			{
				// Update the target object's property based on the keyframe value
				ret = l3d_anim_update_prop(
					&anim->actions[action_idx],
					kf,
					scene,
					anim->target_obj_type,
					anim->target_obj_idx);

				if (ret != L3D_OK)
				{
					L3D_DEBUG_PRINT("Failed to update animation property (%d) value for object type %d ID %d. Error code: %d\n",
						anim->actions[action_idx].property,
						anim->target_obj_type,
						anim->target_obj_idx,
						ret);
					return ret;
				}

				processed_kf_idx++;
			}
			else
			{
				// This keyframe is in the future, break the loop
				break;
			}
		}

		// If there are some keyframes ahead of next_tick_no,
		// apply interpolated value between the last keyframe and the next keyframe...

		const int16_t last_kf_idx = anim->state.processed_kf_indices[action_idx];
		const int16_t next_kf_idx = last_kf_idx + 1;

		// Mark this keyframe as processed if it hasn't been already
		if (processed_kf_idx > anim->state.processed_kf_indices[action_idx])
		{
			anim->state.processed_kf_indices[action_idx] = processed_kf_idx;
			L3D_DEBUG_PRINT("Marked keyframe %d as processed for action %d\n",
				processed_kf_idx, action_idx);
		}

		if (next_kf_idx >= anim->actions[action_idx].keyframe_count)
		{
			// No more keyframes to process for this action
			L3D_DEBUG_PRINT("No interpolation needed for this tick (no more keyframes to process). Continuing.\n");
			continue;
		}

		if (last_kf_idx < 0)
		{
			// No keyframes have been processed yet for this action
			L3D_DEBUG_PRINT("No interpolation needed for this tick (no keyframes have been processed yet). Continuing.\n");
			continue;
		}

		const l3d_keyframe_t *last_kf = &anim->actions[action_idx].keyframes[last_kf_idx];
		const l3d_keyframe_t *next_kf = &anim->actions[action_idx].keyframes[next_kf_idx];

		if (new_tick_no <= last_kf->t || new_tick_no > next_kf->t)
		{
			// The new tick number is outside the range of the last and next keyframes
			L3D_DEBUG_PRINT("No interpolation needed for this tick (new tick number is outside the range of the last and next keyframes). Continuing.\n");
			continue;
		}

		L3D_DEBUG_PRINT("Computing interpolation factor: new_tick_no %d, last_kf_tick %d, next_kf_tick %d\n",
			new_tick_no, last_kf->t, next_kf->t);

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
		l3d_rtnl_t interpolation_factor = l3d_fixedDiv((new_tick_no - last_kf->t),
														(next_kf->t - last_kf->t));
#else
		l3d_rtnl_t interpolation_factor = (l3d_flp_t)((l3d_flp_t)(new_tick_no - last_kf->t) / (l3d_flp_t)(next_kf->t - last_kf->t));
#endif // L3D_USE_FIXED_POINT_ARITHMETIC

		L3D_DEBUG_PRINT("Interpolating between keyframe %d and keyframe %d: factor: %f\n",
			last_kf_idx, next_kf_idx, l3d_rationalToFloat(interpolation_factor));

		// Apply interpolated value to the target object's property
		ret = l3d_anim_update_prop_intp(
			&anim->actions[action_idx],
			last_kf,
			next_kf,
			scene,
			anim->target_obj_type,
			anim->target_obj_idx,
			&interpolation_factor
		);

		if (ret != L3D_OK)
		{
			L3D_DEBUG_PRINT("Failed to update animation property (%d) to interpolated value for object type %d ID %d. Error code: %d\n",
				anim->actions[action_idx].property,
				anim->target_obj_type,
				anim->target_obj_idx,
				ret);
			return ret;
		}
	}

	// Update the current tick number of the animation
	anim->state.current_tick_no = new_tick_no;

	// If the tick has reached or exceeded the last keyframe position
	if (new_tick_no >= anim->last_keyframe_pos)
	{
		if (anim->repeat)
		{
			// Loop back to the first keyframe
			ret = l3d_anim_reset(anim, scene);
			if (ret != L3D_OK)
			{
				L3D_DEBUG_PRINT("Resetting repeating animation failed (%d).\n", ret);
				return ret;
			}
			
			L3D_DEBUG_PRINT("Animation for object type %d ID %d is repeating. Resetting tick number and processed keyframes.\n",
				anim->target_obj_type, anim->target_obj_idx);
		}
		else
		{
			// Animation is finished
			anim->state.is_finished = true;
			anim->state.in_progress = false;
			L3D_DEBUG_PRINT("Animation for object type %d ID %d has finished.\n",
				anim->target_obj_type, anim->target_obj_idx);
		}
	}

	return L3D_OK;
}
