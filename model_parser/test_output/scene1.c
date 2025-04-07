#include "scene1.h"
#include "lib3d_config.h"
#include "lib3d_math.h"

// 
// Scene defines
// 
#define SCENE1_MODEL_VERT_COUNT 13
#define SCENE1_MODEL_FACE_COUNT 18
#define SCENE1_MODEL_EDGE_COUNT 27

// WIP: Calculated by scene-model parser (or however I'm gonna call the script),
// = sum[for each object (model_vertex_count * no_of_object_instances)]
#define SCENE1_TRANSFORMED_VERT_COUNT 13
#define SCENE1_FACE_FLAG_COUNT 18
#define SCENE1_EDGE_FLAG_COUNT 27

#define MESH_CUBE_TRI_INSTANCE_COUNT 1
#define MESH_PYRAMID_TRI_INSTANCE_COUNT 1
#define SCENE1_OBJ_COUNT 2
#define SCENE1_CAM_COUNT 2

// 
// Object defines
// 
#define MESH_CUBE_TRI_VERT_COUNT 8
#define MESH_CUBE_TRI_FACE_COUNT 12
#define MESH_CUBE_TRI_EDGE_COUNT 18
#define MESH_PYRAMID_TRI_VERT_COUNT 5
#define MESH_PYRAMID_TRI_FACE_COUNT 6
#define MESH_PYRAMID_TRI_EDGE_COUNT 9

const fxp_t scene1_model_vertex_data[] = {
	// cube_tri
	-65536, 65536, 65536, 
	-65536, -65536, 65536, 
	-65536, 65536, -65536, 
	-65536, -65536, -65536, 
	65536, 65536, 65536, 
	65536, -65536, 65536, 
	65536, 65536, -65536, 
	65536, -65536, -65536, 
	// pyramid_tri
	65536, 65536, -65536, 
	65536, -65536, -65536, 
	0, 0, 65536, 
	-65536, 65536, -65536, 
	-65536, -65536, -65536, 
};

const uint16_t scene1_model_face_data[] = {
	// cube_tri
	4, 2, 0, 
	2, 7, 3, 
	6, 5, 7, 
	1, 7, 5, 
	0, 3, 1, 
	4, 1, 5, 
	4, 6, 2, 
	2, 6, 7, 
	6, 4, 5, 
	1, 3, 7, 
	0, 2, 3, 
	4, 0, 1, 
	// pyramid_tri
	9, 10, 12, 
	12, 10, 11, 
	8, 12, 11, 
	8, 10, 9, 
	11, 10, 8, 
	8, 9, 12, 
};

const uint16_t scene1_model_edge_data[] = {
	// cube_tri
	4, 2, 0,
	2, 0, 0,
	4, 0, 0,
	2, 7, 1,
	7, 3, 1,
	2, 3, 1,
	6, 5, 2,
	5, 7, 2,
	6, 7, 2,
	1, 7, 3,
	1, 5, 3,
	0, 3, 4,
	3, 1, 4,
	0, 1, 4,
	4, 1, 5,
	4, 5, 5,
	4, 6, 6,
	6, 2, 6,
	// pyramid_tri
	9, 10, 12,
	10, 12, 12,
	9, 12, 12,
	10, 11, 13,
	12, 11, 13,
	8, 12, 14,
	8, 11, 14,
	8, 10, 15,
	8, 9, 15,
};

const uint8_t scene1_edge_flags[] = {
	// cube_tri
	4,
	6,
	6,
	4,
	6,
	6,
	4,
	6,
	6,
	4,
	6,
	4,
	6,
	6,
	4,
	6,
	6,
	6,
	// pyramid_tri
	6,
	6,
	6,
	6,
	6,
	4,
	6,
	6,
	6,
};


l3d_scene_t scene1;
l3d_vec4_t scene1_vertices_world[SCENE1_TRANSFORMED_VERT_COUNT];
l3d_vec4_t scene1_vertices_projected[SCENE1_TRANSFORMED_VERT_COUNT];
uint8_t scene1_face_flags[SCENE1_FACE_FLAG_COUNT];
l3d_obj3d_t scene1_objects[SCENE1_OBJ_COUNT];
l3d_camera_t scene1_cameras[SCENE1_CAM_COUNT];

l3d_err_t init_objects(void) {
	// cube_tri
	for (uint16_t i = 0; i < MESH_CUBE_TRI_INSTANCE_COUNT; i++) {
		scene1_objects[i].mesh.vert_count = MESH_CUBE_TRI_VERT_COUNT;
		scene1_objects[i].mesh.tri_count = MESH_CUBE_TRI_FACE_COUNT;
		scene1_objects[i].mesh.edge_count = MESH_CUBE_TRI_EDGE_COUNT;
	}

	// pyramid_tri
	for (uint16_t i = 0; i < MESH_PYRAMID_TRI_INSTANCE_COUNT; i++) {
		scene1_objects[i].mesh.vert_count = MESH_PYRAMID_TRI_VERT_COUNT;
		scene1_objects[i].mesh.tri_count = MESH_PYRAMID_TRI_FACE_COUNT;
		scene1_objects[i].mesh.edge_count = MESH_PYRAMID_TRI_EDGE_COUNT;
	}


	uint16_t model_vert_data_offset = 0;
	uint16_t model_tri_data_offset = 0;
	uint16_t model_edge_data_offset = 0;
	uint16_t transformed_vertices_offset = 0;
	uint16_t tris_flags_offset = 0;
	uint16_t edges_flags_offset = 0;

	for (uint16_t obj_id = 0; obj_id < SCENE1_OBJ_COUNT; obj_id++) {
		scene1_objects[obj_id].mesh.model_vert_data_offset = model_vert_data_offset;
		scene1_objects[obj_id].mesh.model_tri_data_offset = model_tri_data_offset;
		scene1_objects[obj_id].mesh.model_edge_data_offset = model_edge_data_offset;

		scene1_objects[obj_id].mesh.transformed_vertices_offset = transformed_vertices_offset;
		scene1_objects[obj_id].mesh.tris_flags_offset = tris_flags_offset;	// not used for now
		scene1_objects[obj_id].mesh.edges_flags_offset = edges_flags_offset;

		scene1_objects[obj_id].local_pos = l3d_getZeroVec4();
		scene1_objects[obj_id].local_rot = l3d_getZeroRot();
		// scene1_objects[obj_id].wireframe_colour.value = L3D_COLOUR_WHITE;
		scene1_objects[obj_id].wireframe_colour = L3D_COLOUR_WHITE;
		// scene1_objects[obj_id].fill_colour.value = L3D_COLOUR_WHITE;
		scene1_objects[obj_id].fill_colour = L3D_COLOUR_WHITE;	// to be removed

		// Local orientation unit vectors
		scene1_objects[obj_id].u[0] = l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);
		scene1_objects[obj_id].u[1] = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
		scene1_objects[obj_id].u[2] = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
		scene1_objects[obj_id].u[3] = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
		// (parent, children, group, etc) to be added...

		// Update offsets
		model_vert_data_offset += scene1_objects[obj_id].mesh.vert_count * 3 + 1;
		model_tri_data_offset += scene1_objects[obj_id].mesh.tri_count * 3;
		model_edge_data_offset += scene1_objects[obj_id].mesh.edge_count * 3;

		transformed_vertices_offset += scene1_objects[obj_id].mesh.vert_count + 1;
		tris_flags_offset += scene1_objects[obj_id].mesh.tri_count + 1;
		edges_flags_offset += scene1_objects[obj_id].meshedge_count + 1;
	}

	return L3D_OK;
}

l3d_err_t init_cameras(void) {
	l3d_err_t ret = L3D_OK;
	for (uint16_t i=0; i<SCENE1_CAM_COUNT; i++){
		ret = l3d_cam_reset(&scene1.cameras[i]);

		if (ret != L3D_OK)
			return ret;
	}

	return ret;
}

l3d_err_t scene1_init(void) {
	scene1.model_vert_data = scene1_model_vertex_data;
	scene1.model_tri_data = scene1_model_face_data;
	scene1.model_edge_data = scene1_model_edge_data;
	
	scene1.model_vertex_count = SCENE1_MODEL_VERT_COUNT;
	scene1.model_tri_count = SCENE1_MODEL_FACE_COUNT;
	scene1.model_edge_count = SCENE1_MODEL_EDGE_COUNT;
	
	scene1.vertices_world = scene1_vertices_world;
	scene1.vertices_projected = scene1_vertices_projected;
	
	scene1.tri_flags = scene1_face_flags;
	scene1.edge_flags = scene1_edge_flags;
	
	scene1.transformed_vertex_count = SCENE1_TRANSFORMED_VERT_COUNT;
	scene1.tri_flag_count = SCENE1_FACE_FLAG_COUNT;
	scene1.edge_flag_count = SCENE1_EDGE_FLAG_COUNT;
	
	scene1.objects = scene1_objects;
	scene1.object_count = SCENE1_OBJ_COUNT;
	
	scene1.cameras = scene1_cameras;
	scene1.camera_count = SCENE1_CAM_COUNT;
	
	scene1.active_camera = &scene1.cameras[0];
	
	l3d_err_t ret = init_objects();
	

	if (ret != L3D_OK)
		return ret;

	ret = init_cameras();

	return ret;
}

