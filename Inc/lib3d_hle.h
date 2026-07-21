#ifndef _L3D_HLE_H_
#define _L3D_HLE_H_

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_scene.h"

// The following replace RM(2,50) in Angell's algorithm.

// Maximum number of visible edge fragments after clipping.
// Typical low-poly scenes rarely exceed 2-4 fragments.
// Increase if rendering highly concave meshes.
#define L3D_HLE_MAX_INTERVALS 8

#define L3D_RTNL_ZERO l3d_floatToRational(0.0f)
#define L3D_RTNL_ONE l3d_floatToRational(1.0f)

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

bool l3d_segmentLineIntersection(
	const l3d_vec4_t *p0,
    const l3d_vec4_t *p1,
    const l3d_vec4_t *q0,
    const l3d_vec4_t *q1,
    l3d_rtnl_t *s);
    // const l3d_vec4_t *edge0,
    // const l3d_vec4_t *edge1,
    // const l3d_vec4_t *a,
    // const l3d_vec4_t *b,
    // l3d_rtnl_t *s);

// l3d_err_t l3d_hle_findOverlap(
//     const l3d_scene_t *scene,
//     uint16_t edge_idx,
//     uint16_t face_idx,
//     l3d_rtnl_t *tmin,
//     l3d_rtnl_t *tmax,
//     bool *overlap_found);

bool l3d_hle_findOverlap(
    const l3d_vec4_t *edge0,
    const l3d_vec4_t *edge1,
    const l3d_vec4_t *tri0,
    const l3d_vec4_t *tri1,
    const l3d_vec4_t *tri2,
    l3d_rtnl_t *rmin,
    l3d_rtnl_t *rmax);

// static bool l3d_hle_lineIntersection(
//     const l3d_vec2_t *p1,
//     const l3d_vec2_t *p2,
//     const l3d_vec2_t *q1,
//     const l3d_vec2_t *q2,
//     l3d_rtnl_t *t,
//     l3d_rtnl_t *u);

l3d_vec4_t l3d_vecLerp(const l3d_vec4_t *a, const l3d_vec4_t *b, l3d_rtnl_t f);

void l3d_drawVisibleIntervals(
	const l3d_vec4_t *v0,
	const l3d_vec4_t *v1,
	l3d_interval_list_t *list,
	l3d_colour_t colour);

bool l3d_isPointInTri(
		const l3d_vec4_t *v,
		const l3d_vec4_t *tri_v0,
		const l3d_vec4_t *tri_v1,
		const l3d_vec4_t *tri_v2);

l3d_err_t l3d_render_hle(const l3d_scene_t *scene);

#endif /* _L3D_HLE_H_ */