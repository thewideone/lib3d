#ifndef _L3D_HLE_H_
#define _L3D_HLE_H_

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_scene.h"

// The following replace RM(2,50) in Angell's algorithm.
typedef struct {
	l3d_rtnl_t begin;
	l3d_rtnl_t end;
} l3d_interval_t;

typedef struct {
	l3d_interval_t interval[L3D_HLE_MAX_INTERVALS];
	uint8_t count;
} l3d_interval_list_t;

void l3d_interval_reset(
	l3d_interval_list_t *list);

l3d_err_t l3d_interval_subtract(
	l3d_interval_list_t *list,
	l3d_rtnl_t hidden_begin,
	l3d_rtnl_t hidden_end);

bool l3d_hle_bboxOverlap(
    const l3d_vec4_t *e_v0,
    const l3d_vec4_t *e_v1,
	const l3d_vec4_t *tri_v0,
	const l3d_vec4_t *tri_v1,
	const l3d_vec4_t *tri_v2);

bool l3d_segmentLineIntersection(
	const l3d_vec4_t *p0,
    const l3d_vec4_t *p1,
    const l3d_vec4_t *q0,
    const l3d_vec4_t *q1,
    l3d_rtnl_t *s);

bool l3d_hle_findOverlap(
    const l3d_vec4_t *edge0,
    const l3d_vec4_t *edge1,
    const l3d_vec4_t *tri0,
    const l3d_vec4_t *tri1,
    const l3d_vec4_t *tri2,
    l3d_rtnl_t *rmin,
    l3d_rtnl_t *rmax);

void l3d_drawVisibleIntervals(
	const l3d_vec4_t *v0,
	const l3d_vec4_t *v1,
	l3d_interval_list_t *list,
	l3d_colour_t colour);

bool l3d_hle_edgeBelongsToFace(
	uint16_t edge_v0_idx, uint16_t edge_v1_idx,
	uint16_t tri_v0_idx, uint16_t tri_v1_idx, uint16_t tri_v2_idx);

void l3d_projectEdgeParameter(
    const l3d_vec4_t *world0,
    const l3d_vec4_t *world1,
    const l3d_vec4_t *proj0,
    const l3d_vec4_t *proj1,
	const l3d_mat4x4_t *mat_view,
	const l3d_mat4x4_t *mat_proj,
    l3d_rtnl_t screen_r,
    l3d_rtnl_t *world_r);

l3d_err_t l3d_render_hle(const l3d_scene_t *scene);

#endif /* _L3D_HLE_H_ */