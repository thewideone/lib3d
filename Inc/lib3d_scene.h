#ifndef _L3D_SCENE_H_
#define _L3D_SCENE_H_

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_obj3d.h"
#include "lib3d_camera.h"

typedef struct {
	// All declared here (fixed scene size):
	// cons:
	//  - fixed scene size means big program size even when using only a few small objects
	// 
	// vec4_t model_vertices[L3D_SCENE_MODEL_VERTS_CAP];		// of the original model
	// tri_t tris[L3D_SCENE_FACES_CAP];
	// edge_t edges[L3D_SCENE_EDGES_CAP];
	// 
	// vec4_t vertices_world[L3D_SCENE_VERTS_CAP];		// for hidden line removal
	// vec4_t vertices_projected[L3D_SCENE_VERTS_CAP];	// also for hidden line removal?
	// uint8_t tris_flags[L3D_SCENE_FACES_CAP];
	// uint8_t edges_flags[L3D_SCENE_EDGES_CAP];
	// 
	// obj3d_t objects[L3D_SCENE_OBJ_CAP];

	// Pointers to automatically declared arrays in scene description files:
	// pros:
	//  - all arrays defined in scene description files - many different scenes can be loaded
	//  - program size corresponds to number of objects of scene

	// Const, common for all instances of all objects in the scene,
	// contain unmodified data of all objects in the scene
	const l3d_fxp_t *model_vert_data;		// of the original model
	const uint16_t *model_tri_data;
	const uint16_t *model_edge_data;

	uint16_t model_vertex_count;
	uint16_t model_tri_count;
	uint16_t model_edge_count;

	// Sizes of these arrays depend on the number of instances of each object in the scene
	l3d_vec4_t *vertices_world;		// stored for later use in hidden line removal algorithms
	l3d_vec4_t *vertices_projected;	// also for hidden line removal?
	uint8_t *tri_flags;
	uint8_t *edge_flags;

	uint16_t transformed_vertex_count;	// = sum[for each object (model_vertex_count	* no_of_object_instances)]
	uint16_t tri_flag_count;			// = sum[for each object (model_tri_count		* no_of_object_instances)]
	uint16_t edge_flag_count;			// = sum[for each object (model_edge_count		* no_of_object_instances)]

	l3d_obj3d_t *objects;
	uint16_t object_count;

	l3d_camera_t *cameras;
	l3d_camera_t *active_camera;
	uint16_t camera_count;

	l3d_mat4x4_t mat_proj;	// projection matrix
	l3d_mat4x4_t mat_view;	// view matrix

	// light sources?

} l3d_scene_t;

// listObjects(): object name?; number of instances; is visible?; location?
// listCameras()
// listUsedGroups()?

// To modify (e.g. animate) an object:
// getObject() <- get by what? name/ID/indexInTheArray/name+instanceID/?
// At least for now by index
l3d_obj3d_t *l3d_scene_getObjectByIdx(l3d_scene_t *scene, uint16_t idx);

// To modify (e.g. set active, animate) a camera:
// getCamera() <- get by what?
l3d_camera_t *l3d_scene_getActiveCamera(l3d_scene_t *scene);
l3d_err_t l3d_scene_setActiveCamera(l3d_scene_t *scene, l3d_camera_t *cam);

l3d_vec4_t l3d_scene_getObjectLocalPos(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx);
l3d_err_t l3d_scene_setObjectLocalPos(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_vec4_t *pos);
l3d_rot_t l3d_scene_getObjectLocalRot(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx);
l3d_err_t l3d_scene_setObjectLocalRot(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx, const l3d_rot_t *rot);

l3d_vec4_t l3d_scene_getObjectLocalUnitVecX(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx);
l3d_vec4_t l3d_scene_getObjectLocalUnitVecY(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx);
l3d_vec4_t l3d_scene_getObjectLocalUnitVecZ(const l3d_scene_t *scene, l3d_obj_type_t type, uint16_t idx);
// No setters - they would be unsafe


#endif	// _L3D_SCENE_H_