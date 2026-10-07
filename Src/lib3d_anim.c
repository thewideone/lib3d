#include "lib3d_config.h"
#include "lib3d_anim.h"
#include "lib3d_util.h"
#include "lib3d_math.h"
#include "lib3d_scene.h"
#include "lib3d_transform.h"


l3d_err_t l3d_anim_init(
	// l3d_anim_state_t *state,
	l3d_anim_t *anim)
{
	if (anim == NULL)
		return L3D_WRONG_PARAM;
	
	// anim->state.anim = anim;
	// anim->state.current_action_idx = 0;
	// anim->state.current_keyframe_idx = 0;
	// anim->state.current_tick_no = 0;
	// anim->state.in_progress = false;
	// anim->state.is_finished = false;

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

	return L3D_OK;
}

l3d_err_t l3d_anim_reset(l3d_anim_t *anim)
{
	if (anim == NULL)
		return L3D_WRONG_PARAM;

	anim->state.current_tick_no = 0;
	anim->state.in_progress = false;
	anim->state.is_finished = false;

	// Reset processed keyframe indices for each action
	for (uint8_t action_idx = 0; action_idx < anim->action_count; action_idx++)
	{
		anim->state.processed_kf_indices[action_idx] = -1;
	}

	return L3D_OK;
}

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

	uint16_t new_tick_no = anim->state.current_tick_no + ticks_elapsed;
	if (!anim->state.in_progress)
	{
		anim->state.in_progress = true;
		anim->state.is_finished = false;
		new_tick_no = 0;	// start from the beginning
		L3D_DEBUG_PRINT("Animation for object type %d ID %d started.\n",
			anim->target_obj_type, anim->target_obj_idx);
	}

	l3d_err_t ret = L3D_OK;
	// uint16_t new_tick_no = anim->state.current_tick_no + ticks_elapsed;

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

		L3D_DEBUG_PRINT("Processing action %d for object type %d ID %d at tick %d\n",
			action_idx, anim->target_obj_type, anim->target_obj_idx, new_tick_no);

		// For each unprocessed keyframe in the action up to the new tick number,
		// apply the keyframe value to the target object's property
		int16_t kf_idx = anim->state.processed_kf_indices[action_idx];
		if (kf_idx < 0)
			kf_idx = 0;	// skip invalid index

		for (kf_idx;
			kf_idx < anim->actions[action_idx].keyframe_count;
			kf_idx++)
		{
			const l3d_keyframe_t *kf = &anim->actions[action_idx].keyframes[kf_idx];

			L3D_DEBUG_PRINT("Checking keyframe %d at tick %d for action %d\n",
				kf_idx, kf->t, action_idx);

			// If the keyframe's tick is less than or equal to the new tick number
			if (kf->t <= new_tick_no)
			{
				// Update the target object's property based on the keyframe value
				l3d_rtnl_t current_prop_value, delta_value;
				switch (anim->actions[action_idx].property)
				{
					case L3D_ANIM_PROP_POS_GLOBAL_X:
						current_prop_value = l3d_scene_getObjectLocalPos(scene, anim->target_obj_type, anim->target_obj_idx).x;
						delta_value = kf->value - current_prop_value;

						if (l3d_abs(delta_value) < L3D_EPSILON_RTNL )
							break;	// no need to update if the value is already close enough

						L3D_DEBUG_PRINT("Updating global X position for object type %d ID %d by value %f\n",
							anim->target_obj_type, anim->target_obj_idx, l3d_rationalToFloat(delta_value));
						
						ret = l3d_moveGlobalX(scene, anim->target_obj_type, anim->target_obj_idx, delta_value);
						if (ret != L3D_OK)
						{
							L3D_DEBUG_PRINT("Failed to set object local position for object type %d ID %d. Error code: %d\n",
								anim->target_obj_type, anim->target_obj_idx, ret);
							return ret;
						}
						break;
					case L3D_ANIM_PROP_POS_GLOBAL_Y:
						current_prop_value = l3d_scene_getObjectLocalPos(scene, anim->target_obj_type, anim->target_obj_idx).y;
						delta_value = kf->value - current_prop_value;

						if (l3d_abs(delta_value) < L3D_EPSILON_RTNL)
							break;	// no need to update if the value is already close enough

						L3D_DEBUG_PRINT("Updating global Y position for object type %d ID %d by value %f\n",
							anim->target_obj_type, anim->target_obj_idx, l3d_rationalToFloat(delta_value));
						
						ret = l3d_moveGlobalY(scene, anim->target_obj_type, anim->target_obj_idx, delta_value);
						if (ret != L3D_OK)
						{
							L3D_DEBUG_PRINT("Failed to set object local position for object type %d ID %d. Error code: %d\n",
								anim->target_obj_type, anim->target_obj_idx, ret);
							return ret;
						}
						break;
					default:
						return L3D_WRONG_PARAM;
				}

				// Mark this keyframe as processed if it hasn't been already
				if (anim->state.processed_kf_indices[action_idx] < kf_idx)
				{
					anim->state.processed_kf_indices[action_idx] = kf_idx;
					L3D_DEBUG_PRINT("Marked keyframe %d as processed for action %d\n", kf_idx, action_idx);
				}
			}
			else
			{
				// This keyframe is in the future, break the loop
				break;
			}
		}

		// If there are some keyframes ahead of next_tick_no,
		// apply interpolated value between the last keyframe and the next keyframe

		const int16_t last_kf_idx = anim->state.processed_kf_indices[action_idx];
		const int16_t next_kf_idx = last_kf_idx + 1;

		if (next_kf_idx >= anim->actions[action_idx].keyframe_count)
		{
			// No more keyframes to process for this action
			continue;
		}

		if (last_kf_idx < 0)
		{
			// No keyframes have been processed yet for this action
			continue;
		}

		const l3d_keyframe_t *last_kf = &anim->actions[action_idx].keyframes[last_kf_idx];
		const l3d_keyframe_t *next_kf = &anim->actions[action_idx].keyframes[next_kf_idx];

		if (new_tick_no <= last_kf->t || new_tick_no > next_kf->t)
		{
			// The new tick number is outside the range of the last and next keyframes
			continue;
		}

		L3D_DEBUG_PRINT("Computing factor: new_tick_no %d, last_kf_tick %d, next_kf_tick %d for action %d\n",
			new_tick_no, last_kf->t, next_kf->t, action_idx);

#ifdef L3D_USE_FIXED_POINT_ARITHMETIC
		l3d_rtnl_t interpolation_factor = l3d_fixedDiv((new_tick_no - last_kf->t),
														(next_kf->t - last_kf->t));
#else
		l3d_rtnl_t interpolation_factor = (l3d_flp_t)((l3d_flp_t)(new_tick_no - last_kf->t) / (l3d_flp_t)(next_kf->t - last_kf->t));
#endif // L3D_USE_FIXED_POINT_ARITHMETIC

		L3D_DEBUG_PRINT("Interpolating between keyframe %d and keyframe %d for action %d: factor: %f\n",
			last_kf_idx, next_kf_idx, action_idx, l3d_rationalToFloat(interpolation_factor));
		
		L3D_DEBUG_PRINT("Interpolating global position for object type %d ID %d\n",
			anim->target_obj_type, anim->target_obj_idx);

		// Apply interpolated value to the target object's property
		l3d_rtnl_t last_prop_value, current_prop_value, next_prop_value, interpolated_value, delta_value;
		switch (anim->actions[action_idx].property)
		{
			case L3D_ANIM_PROP_POS_GLOBAL_X:
				last_prop_value = last_kf->value; //last_kf->target_vec4.x;	// or other
				current_prop_value = l3d_scene_getObjectLocalPos(scene, anim->target_obj_type, anim->target_obj_idx).x;
				next_prop_value = next_kf->value; //next_kf->target_vec4.x;	// or other

				// switch (next_kf->interpolation)...
				// switch (next_kf->easing)...
				// ...

				interpolated_value = l3d_lerp(last_prop_value, next_prop_value, interpolation_factor);
				delta_value = interpolated_value - current_prop_value;

				ret = l3d_moveGlobalX(scene, anim->target_obj_type, anim->target_obj_idx, delta_value);
				break;
			case L3D_ANIM_PROP_POS_GLOBAL_Y:
				last_prop_value = last_kf->value; //last_kf->target_vec4.y;	// or other
				current_prop_value = l3d_scene_getObjectLocalPos(scene, anim->target_obj_type, anim->target_obj_idx).y;
				next_prop_value = next_kf->value; //next_kf->target_vec4.y;	// or other

				// switch (next_kf->interpolation)...
				// switch (next_kf->easing)...
				// ...

				interpolated_value = l3d_lerp(last_prop_value, next_prop_value, interpolation_factor);
				delta_value = interpolated_value - current_prop_value;
				
				ret = l3d_moveGlobalY(scene, anim->target_obj_type, anim->target_obj_idx, delta_value);
				break;
			default:
				return L3D_WRONG_PARAM;
		}

		L3D_DEBUG_PRINT("current %f, next %f, interpolated %f, delta %f\n",
					l3d_rationalToFloat(current_prop_value),
					l3d_rationalToFloat(next_prop_value),
					l3d_rationalToFloat(interpolated_value),
					l3d_rationalToFloat(delta_value));

		if (ret != L3D_OK)
		{
			L3D_DEBUG_PRINT("Failed to set object local position for object type %d ID %d. Error code: %d\n",
				anim->target_obj_type, anim->target_obj_idx, ret);
			return ret;
		}
	}

	// Update the current tick number of the animation
	anim->state.current_tick_no = new_tick_no;

	if (new_tick_no >= anim->last_keyframe_pos)
	{
		if (anim->repeat)
		{
			// Loop back to the first keyframe
			anim->state.current_tick_no = 0;
			anim->state.in_progress = false;	// not in progress as the anim has just finished and will restart on the next update
			for (uint8_t action_idx = 0; action_idx < anim->action_count; action_idx++)
			{
				anim->state.processed_kf_indices[action_idx] = -1; // reset processed keyframes
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

	// // For each action (and its property)
	// for (uint8_t action_idx = 0; action_idx < anim->action_count; action_idx++)
	// {
	// 	// // Compute number of frames since last keyframe
	// 	// int32_t frames_elapsed = current_frame - anim->actions[action_idx].keyframes[anim->state.current_keyframe_idx].t;
		
	// 	// If no keyframes for this action, skip it
	// 	if (anim->actions[action_idx].keyframe_count == 0)
	// 	{
	// 		L3D_DEBUG_PRINT("Animation for object type %d ID %d has action %d with no keyframes. Skipping.\n",
	// 			anim->target_obj_idx, anim->target_obj_type, action_idx);
	// 		continue;
	// 	}

	// 	// Compute case
	// 	// uint8_t case_number = 0;

	// 	// If current tick number is before the first keyframe,
	// 	// do nothing.
	// 	if (anim->state.current_tick_no < anim->actions[action_idx].keyframes[0].t)
	// 	{
	// 		continue;
	// 	}

	// 	uint16_t last_keyframe_idx = anim->actions[action_idx].keyframe_count - 1;
		
	// 	// If current tick number is after the last keyframe
	// 	if (anim->state.current_tick_no > anim->actions[action_idx].keyframes[last_keyframe_idx].t)
	// 	{
	// 		if (anim->repeat)
	// 		{
	// 			// Loop back to the first keyframe
	// 			// state->current_keyframe_idx = 0;
	// 			L3D_DEBUG_PRINT("Action %d: animation repeat not implemented yet. Skipping.\n", action_idx);
	// 			continue;
	// 		}
	// 		else
	// 		{
	// 			// Current action is finished
	// 			// anim->action_flags[action_idx] = L3D_ANIM_ACTION_FLAG_FINISHED;
	// 			// Animation is finished if // all its actions are finished
	// 			// the last keyframe of all actions has been reached
				
	// 			// state->is_finished = true;
	// 			// state->in_progress = false;
	// 			continue;
	// 		}
	// 	}

	// 	// Search for last keyframe that is less than or equal to current_tick_no
	// 	for (uint16_t tick_itr = anim->actions[action_idx].keyframes[0].t,
	// 		frames_encountered = 0;
	// 		tick_itr <= anim->actions[action_idx].keyframes[last_keyframe_idx].t;
	// 		tick_itr++)
	// 	{
	// 		// Current tick is at a keyframe
	// 		if (tick_itr == anim->state.current_tick_no)
	// 		{
	// 			// Set target object property to the keyframe value
	// 			// ...
	// 			break;
	// 		}
	// 		// if (frames_encountered < )
	// 		// {

	// 		// }
	// 	}
		
	// 	// Compute delta value 


	// 	// switch (state->anim->actions[state->current_action_idx].property)
	// 	// {
	// 	// 	case L3D_ANIM_POS_GLOBAL_X:
	// 	// 		// Update the object's global X position based on the current keyframe value
	// 	// 		l3d_moveGlobalX(scene, state->anim->target_obj_type, state->anim->target_obj_idx,
	// 	// 			state->anim->actions[state->current_action_idx].keyframes[state->current_keyframe_idx].value);
	// 	// 		break;

	// 	// 	case L3D_ANIM_POS_GLOBAL_Y:
	// 	// 		// Update the object's global Y position based on the current keyframe value
	// 	// 		l3d_moveGlobalY(scene, state->anim->target_obj_type, state->anim->target_obj_idx,
	// 	// 			state->anim->actions[state->current_action_idx].keyframes[state->current_keyframe_idx].value);
	// 	// 		break;

	// 	// 	default:
	// 	// 		return L3D_WRONG_PARAM;
	// 	// }
	// }
	
}
