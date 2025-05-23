// 
// Main header of the 3D graphics library; to be included by superproject.
// 

#ifndef _L3D_CORE_H_
#define _L3D_CORE_H_

#include "lib3d_config.h"
#include "lib3d_util.h"
#include "lib3d_scene.h"
#include "lib3d_transform.h"

void l3d_initGlobalAxesMarker(void);
void l3d_transformGlobalAxesMarkerIntoViewSpace(const l3d_mat4x4_t *mat_view, const l3d_mat4x4_t *mat_proj);
l3d_err_t l3d_drawGlobalAxesMarker(void);

void l3d_makeProjectionMatrix(l3d_mat4x4_t *mat, const l3d_camera_t *cam);
void l3d_computeViewMatrix(l3d_camera_t *cam, l3d_mat4x4_t *mat_view);

// void l3d_transformObjectIntoViewSpace(l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx);

// l3d_err_t l3d_drawWireframe(const l3d_scene_t *scene, uint16_t obj_id);
// l3d_err_t l3d_drawObjects(const l3d_scene_t *scene);

l3d_err_t l3d_setupObjects(l3d_scene_t *scene);
l3d_err_t l3d_processScene(l3d_scene_t *scene);

extern void l3d_drawLineCallback(int32_t x0, int32_t y0, int32_t x1, int32_t y1, l3d_colour_t colour);

extern void l3d_putTextCallback(int32_t x, int32_t y, char* str, l3d_colour_t colour);
extern void l3d_putUInt32Callback(int32_t x, int32_t y, uint32_t num, uint8_t digits_cnt, l3d_colour_t colour);
extern void l3d_putInt32Callback(int32_t x, int32_t y, int32_t num, uint8_t digits_cnt, l3d_colour_t colour);
// extern void l3d_putRationalCallback( int32_t x, int32_t y, l3d_rtnl_t num, l3d_colour_t colour);

#endif // _L3D_CORE_H_