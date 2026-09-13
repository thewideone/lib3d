#include "scene_A.h"
#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_core.h" // for l3d_setupObjects()

// 
// Scene defines
// 
#define SCENE_A_MODEL_VERT_COUNT 18
#define SCENE_A_MODEL_FACE_COUNT 36
#define SCENE_A_MODEL_EDGE_COUNT 54

// WIP: Calculated by scene-model parser (or however I'm gonna call the script),
// = sum[for each object (model_vertex_count * no_of_object_instances)]
#define SCENE_A_TRANSFORMED_VERT_COUNT 18
#define SCENE_A_FACE_FLAG_COUNT 36
#define SCENE_A_EDGE_FLAG_COUNT 54

// 
// Object defines
// 
#define MESH_A_VERT_COUNT 18
#define MESH_A_FACE_COUNT 36
#define MESH_A_EDGE_COUNT 54

const int8_t scene_A_model_vertex_data[] = {
	// A
	3, 0, 3, 
	0, 0, -4, 
	0, 1, -4, 
	3, 1, 3, 
	2, 0, 4, 
	2, 1, 4, 
	1, 0, 2, 
	1, 1, 2, 
	-2, 0, 4, 
	-2, 1, 4, 
	-3, 0, 4, 
	-3, 1, 4, 
	0, 0, -1, 
	1, 0, 1, 
	0, 1, -1, 
	1, 1, 1, 
	-1, 0, 1, 
	-1, 1, 1, 
};

const uint16_t scene_A_model_face_data[] = {
	// A
	0, 1, 2, 
	0, 2, 3, 
	4, 0, 3, 
	4, 3, 5, 
	6, 4, 5, 
	6, 5, 7, 
	8, 6, 7, 
	8, 7, 9, 
	10, 8, 9, 
	10, 9, 11, 
	1, 10, 11, 
	1, 11, 2, 
	12, 13, 14, 
	14, 13, 15, 
	13, 16, 15, 
	15, 16, 17, 
	16, 12, 17, 
	17, 12, 14, 
	10, 16, 8, 
	16, 6, 8, 
	6, 0, 4, 
	6, 13, 0, 
	16, 13, 6, 
	12, 1, 13, 
	16, 1, 12, 
	10, 1, 16, 
	13, 1, 0, 
	17, 11, 9, 
	7, 17, 9, 
	3, 7, 5, 
	15, 7, 3, 
	15, 17, 7, 
	2, 14, 15, 
	2, 17, 14, 
	2, 11, 17, 
	2, 15, 3, 
};

const uint16_t scene_A_model_edge_data[] = {
	// A
	0, 1, 0,
	1, 2, 0,
	0, 2, 0,
	2, 3, 1,
	0, 3, 1,
	4, 0, 2,
	4, 3, 2,
	3, 5, 3,
	4, 5, 3,
	6, 4, 4,
	6, 5, 4,
	5, 7, 5,
	6, 7, 5,
	8, 6, 6,
	8, 7, 6,
	7, 9, 7,
	8, 9, 7,
	10, 8, 8,
	10, 9, 8,
	9, 11, 9,
	10, 11, 9,
	1, 10, 10,
	1, 11, 10,
	11, 2, 11,
	12, 13, 12,
	13, 14, 12,
	12, 14, 12,
	13, 15, 13,
	14, 15, 13,
	13, 16, 14,
	16, 15, 14,
	16, 17, 15,
	15, 17, 15,
	16, 12, 16,
	12, 17, 16,
	17, 14, 17,
	10, 16, 18,
	16, 8, 18,
	16, 6, 19,
	6, 0, 20,
	6, 13, 21,
	13, 0, 21,
	12, 1, 23,
	1, 13, 23,
	16, 1, 24,
	17, 11, 27,
	17, 9, 27,
	7, 17, 28,
	3, 7, 29,
	15, 7, 30,
	15, 3, 30,
	2, 14, 32,
	2, 15, 32,
	2, 17, 33,
};

uint8_t scene_A_edge_flags[] = {
	// A instance 0
	14,
	14,
	12,
	14,
	14,
	14,
	12,
	14,
	14,
	14,
	12,
	14,
	14,
	14,
	12,
	14,
	14,
	14,
	12,
	14,
	14,
	14,
	12,
	14,
	14,
	12,
	14,
	14,
	14,
	14,
	12,
	14,
	14,
	14,
	12,
	14,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
	12,
};


l3d_scene_t scene_A;
l3d_vec4_t scene_A_vertices_world[SCENE_A_TRANSFORMED_VERT_COUNT];
l3d_vec4_t scene_A_vertices_projected[SCENE_A_TRANSFORMED_VERT_COUNT];
uint8_t scene_A_face_flags[SCENE_A_FACE_FLAG_COUNT];
l3d_obj3d_t scene_A_objects[SCENE_A_OBJ_COUNT];
l3d_scene_instance_desc_t scene_A_mesh_instances[SCENE_A_MESH_COUNT];
l3d_camera_t scene_A_cameras[SCENE_A_CAM_COUNT];

static l3d_err_t init_objects(void) {
	// A
	for (uint16_t i = scene_A_mesh_instances[0].first_instance_idx;
		i < scene_A_mesh_instances[0].first_instance_idx + SCENE_A_OBJ_A_INSTANCE_COUNT;
		i++) {
		scene_A_objects[i].mesh.vert_count = MESH_A_VERT_COUNT;
		scene_A_objects[i].mesh.tri_count = MESH_A_FACE_COUNT;
		scene_A_objects[i].mesh.edge_count = MESH_A_EDGE_COUNT;
	}


	uint16_t model_vert_data_offset = 0;
	uint16_t model_tri_data_offset = 0;
	uint16_t model_edge_data_offset = 0;
	uint16_t transformed_vertices_offset = 0;
	uint16_t tris_flags_offset = 0;
	uint16_t edges_flags_offset = 0;

	for (uint16_t i = 0; i < SCENE_A_MESH_COUNT; i++) {
		for (uint16_t instance_idx = 0; instance_idx < scene_A_mesh_instances[i].instance_count; instance_idx++) {
			uint16_t obj_id = scene_A_mesh_instances[i].first_instance_idx + instance_idx;
			scene_A_objects[obj_id].mesh.model_vert_data_offset = model_vert_data_offset;
			scene_A_objects[obj_id].mesh.model_tri_data_offset = model_tri_data_offset;
			scene_A_objects[obj_id].mesh.model_edge_data_offset = model_edge_data_offset;

			scene_A_objects[obj_id].mesh.transformed_vertices_offset = transformed_vertices_offset;
			scene_A_objects[obj_id].mesh.tris_flags_offset = tris_flags_offset;	// not used for now
			scene_A_objects[obj_id].mesh.edges_flags_offset = edges_flags_offset;

			transformed_vertices_offset += scene_A_objects[obj_id].mesh.vert_count;
			tris_flags_offset += scene_A_objects[obj_id].mesh.tri_count;
			edges_flags_offset += scene_A_objects[obj_id].mesh.edge_count;
		}

		// Update offsets
		model_vert_data_offset += scene_A_objects[scene_A_mesh_instances[i].first_instance_idx].mesh.vert_count * 3; // check correctness
		model_tri_data_offset += scene_A_objects[scene_A_mesh_instances[i].first_instance_idx].mesh.tri_count * 3; // check correctness
		model_edge_data_offset += scene_A_objects[scene_A_mesh_instances[i].first_instance_idx].mesh.edge_count * 3; // check correctness
	}

	// Common for all objects
	for (uint16_t obj_id = 0; obj_id < SCENE_A_OBJ_COUNT; obj_id++) {
		scene_A_objects[obj_id].local_pos = l3d_getZeroVec4();
		scene_A_objects[obj_id].orientation = l3d_getIdentityQuat();
		// scene_A_objects[obj_id].wireframe_colour.value = L3D_COLOUR_WHITE;
		scene_A_objects[obj_id].wireframe_colour = L3D_COLOUR_WHITE;

		// Local orientation unit vectors
		scene_A_objects[obj_id].u[0] = l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);
		scene_A_objects[obj_id].u[1] = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
		scene_A_objects[obj_id].u[2] = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
		scene_A_objects[obj_id].u[3] = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
		// (parent, children, group, etc) to be added...
	}

	return L3D_OK;
}

static l3d_err_t init_cameras(void) {
	l3d_err_t ret = L3D_OK;
	for (uint16_t i=0; i<SCENE_A_CAM_COUNT; i++){
		ret = l3d_cam_reset(&scene_A.cameras[i]);

		if (ret != L3D_OK)
			return ret;
	}

	return ret;
}

l3d_err_t scene_A_init(void) {
	scene_A.model_vert_data = scene_A_model_vertex_data;
	scene_A.model_tri_data = scene_A_model_face_data;
	scene_A.model_edge_data = scene_A_model_edge_data;
	
	scene_A.model_vertex_count = SCENE_A_MODEL_VERT_COUNT;
	scene_A.model_tri_count = SCENE_A_MODEL_FACE_COUNT;
	scene_A.model_edge_count = SCENE_A_MODEL_EDGE_COUNT;
	
	scene_A.vertices_world = scene_A_vertices_world;
	scene_A.vertices_projected = scene_A_vertices_projected;
	
	scene_A.tri_flags = scene_A_face_flags;
	scene_A.edge_flags = scene_A_edge_flags;
	
	scene_A.transformed_vertex_count = SCENE_A_TRANSFORMED_VERT_COUNT;
	scene_A.tri_flag_count = SCENE_A_FACE_FLAG_COUNT;
	scene_A.edge_flag_count = SCENE_A_EDGE_FLAG_COUNT;
	
	scene_A.objects = scene_A_objects;
	scene_A.object_count = SCENE_A_OBJ_COUNT;
	
	// A
	scene_A_mesh_instances[0].first_instance_idx = 0;
	scene_A_mesh_instances[0].instance_count = SCENE_A_OBJ_A_INSTANCE_COUNT;
	
	scene_A.cameras = scene_A_cameras;
	scene_A.camera_count = SCENE_A_CAM_COUNT;
	
	scene_A.active_camera_idx = 0;
	
	l3d_err_t ret = init_objects();

	if (ret != L3D_OK)
		return ret;

	ret = init_cameras();

	if (ret != L3D_OK)
		return ret;
	
	l3d_makeProjectionMatrix(&scene_A.mat_proj, l3d_scene_getActiveCamera(&scene_A));
	l3d_computeViewMatrix(l3d_scene_getActiveCamera(&scene_A), &(scene_A.mat_view));
	return l3d_setupObjects(&scene_A);
}

