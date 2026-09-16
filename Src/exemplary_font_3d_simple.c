#include "exemplary_font_3d_simple.h"
#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_core.h" // for l3d_setupObjects()

// 
// Scene defines
// 
#define FONT_3D_SIMPLE_MODEL_VERT_COUNT 66
#define FONT_3D_SIMPLE_MODEL_FACE_COUNT 132
#define FONT_3D_SIMPLE_MODEL_EDGE_COUNT 198

// WIP: Calculated by scene-model parser (or however I'm gonna call the script),
// = sum[for each object (model_vertex_count * no_of_object_instances)]
#define FONT_3D_SIMPLE_TRANSFORMED_VERT_COUNT 66
#define FONT_3D_SIMPLE_FACE_FLAG_COUNT 132
#define FONT_3D_SIMPLE_EDGE_FLAG_COUNT 198

// 
// Object defines
// 
#define MESH_A_VERT_COUNT 18
#define MESH_A_FACE_COUNT 36
#define MESH_A_EDGE_COUNT 54
#define MESH_B_VERT_COUNT 26
#define MESH_B_FACE_COUNT 56
#define MESH_B_EDGE_COUNT 84
#define MESH_C_VERT_COUNT 22
#define MESH_C_FACE_COUNT 40
#define MESH_C_EDGE_COUNT 60

const uint16_t font_3d_simple_model_vert_counts[] = {
	MESH_A_VERT_COUNT,
	MESH_B_VERT_COUNT,
	MESH_C_VERT_COUNT
};

const uint16_t font_3d_simple_model_face_counts[] = {
	MESH_A_FACE_COUNT,
	MESH_B_FACE_COUNT,
	MESH_C_FACE_COUNT
};

const uint16_t font_3d_simple_model_edge_counts[] = {
	MESH_A_EDGE_COUNT,
	MESH_B_EDGE_COUNT,
	MESH_C_EDGE_COUNT
};

const int8_t font_3d_simple_model_vertex_data[] = {
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
	// B
	-1, 0, -4, 
	-2, 1, -4, 
	-1, 1, -4, 
	-2, 0, -4, 
	1, 0, -2, 
	-1, 0, -3, 
	0, 0, -2, 
	-1, 1, -3, 
	0, 1, -2, 
	-1, 0, -1, 
	0, 0, 0, 
	1, 1, -2, 
	0, 1, 0, 
	-1, 0, 1, 
	1, 0, 2, 
	2, 0, 1, 
	1, 0, 4, 
	2, 1, 1, 
	1, 1, 4, 
	1, 1, 2, 
	-1, 0, 3, 
	-1, 1, 3, 
	-1, 1, 1, 
	-2, 1, 4, 
	-1, 1, -1, 
	-2, 0, 4, 
	// C
	0, 0, -4, 
	-2, 0, -1, 
	2, 0, -3, 
	1, 0, -2, 
	0, 0, -2, 
	-2, 1, -1, 
	0, 1, -4, 
	2, 1, -3, 
	1, 1, -2, 
	0, 1, -2, 
	-1, 1, 0, 
	-1, 0, 0, 
	0, 0, 2, 
	-2, 0, 1, 
	-2, 1, 1, 
	0, 0, 4, 
	0, 1, 4, 
	0, 1, 2, 
	1, 1, 2, 
	2, 1, 3, 
	2, 0, 3, 
	1, 0, 2, 
};

const uint16_t font_3d_simple_model_face_data[] = {
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
	// B
	0, 1, 2, 
	0, 3, 1, 
	4, 0, 2, 
	3, 0, 5, 
	0, 4, 6, 
	5, 0, 6, 
	7, 6, 8, 
	5, 6, 7, 
	6, 9, 8, 
	9, 6, 10, 
	6, 4, 10, 
	10, 11, 12, 
	10, 4, 11, 
	15, 10, 12, 
	9, 10, 13, 
	13, 10, 14, 
	10, 15, 14, 
	14, 15, 16, 
	16, 15, 17, 
	15, 12, 17, 
	16, 17, 18, 
	17, 19, 18, 
	17, 12, 19, 
	12, 22, 19, 
	19, 21, 18, 
	19, 20, 21, 
	14, 20, 19, 
	22, 14, 19, 
	12, 24, 22, 
	21, 13, 22, 
	13, 14, 22, 
	1, 23, 22, 
	22, 23, 21, 
	1, 22, 24, 
	24, 5, 7, 
	9, 5, 24, 
	7, 1, 24, 
	8, 9, 24, 
	8, 24, 12, 
	18, 21, 23, 
	25, 18, 23, 
	1, 25, 23, 
	25, 16, 18, 
	25, 13, 20, 
	20, 16, 25, 
	25, 3, 13, 
	3, 25, 1, 
	20, 13, 21, 
	20, 14, 16, 
	13, 3, 9, 
	11, 8, 12, 
	4, 2, 11, 
	11, 2, 8, 
	3, 5, 9, 
	2, 7, 8, 
	2, 1, 7, 
	// C
	1, 0, 4, 
	0, 1, 5, 
	0, 2, 3, 
	2, 0, 6, 
	4, 0, 3, 
	0, 5, 6, 
	2, 6, 7, 
	7, 6, 8, 
	6, 9, 8, 
	6, 5, 9, 
	9, 5, 10, 
	4, 8, 9, 
	11, 9, 10, 
	11, 4, 9, 
	12, 11, 10, 
	13, 11, 12, 
	1, 11, 13, 
	1, 4, 11, 
	13, 12, 15, 
	1, 13, 14, 
	13, 16, 14, 
	13, 15, 16, 
	18, 17, 16, 
	17, 14, 16, 
	19, 18, 16, 
	15, 19, 16, 
	20, 18, 19, 
	15, 20, 19, 
	21, 20, 15, 
	20, 21, 18, 
	21, 17, 18, 
	21, 12, 17, 
	12, 21, 15, 
	10, 14, 17, 
	12, 10, 17, 
	10, 5, 14, 
	1, 14, 5, 
	3, 7, 8, 
	4, 3, 8, 
	3, 2, 7, 
};

const uint16_t font_3d_simple_model_edge_data[] = {
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
	// B
	0, 1, 0,
	1, 2, 0,
	0, 2, 0,
	0, 3, 1,
	3, 1, 1,
	4, 0, 2,
	4, 2, 2,
	0, 5, 3,
	3, 5, 3,
	4, 6, 4,
	0, 6, 4,
	5, 6, 5,
	7, 6, 6,
	6, 8, 6,
	7, 8, 6,
	5, 7, 7,
	6, 9, 8,
	9, 8, 8,
	6, 10, 9,
	9, 10, 9,
	4, 10, 10,
	10, 11, 11,
	11, 12, 11,
	10, 12, 11,
	4, 11, 12,
	15, 10, 13,
	15, 12, 13,
	10, 13, 14,
	9, 13, 14,
	10, 14, 15,
	13, 14, 15,
	15, 14, 16,
	15, 16, 17,
	14, 16, 17,
	15, 17, 18,
	16, 17, 18,
	12, 17, 19,
	17, 18, 20,
	16, 18, 20,
	17, 19, 21,
	19, 18, 21,
	12, 19, 22,
	12, 22, 23,
	22, 19, 23,
	19, 21, 24,
	21, 18, 24,
	19, 20, 25,
	20, 21, 25,
	14, 20, 26,
	14, 19, 26,
	22, 14, 27,
	12, 24, 28,
	24, 22, 28,
	21, 13, 29,
	13, 22, 29,
	21, 22, 29,
	1, 23, 31,
	23, 22, 31,
	1, 22, 31,
	23, 21, 32,
	1, 24, 33,
	24, 5, 34,
	24, 7, 34,
	9, 5, 35,
	9, 24, 35,
	7, 1, 36,
	8, 24, 37,
	8, 12, 38,
	18, 23, 39,
	25, 18, 40,
	25, 23, 40,
	1, 25, 41,
	25, 16, 42,
	25, 13, 43,
	13, 20, 43,
	25, 20, 43,
	20, 16, 44,
	25, 3, 45,
	3, 13, 45,
	3, 9, 49,
	11, 8, 50,
	2, 11, 51,
	2, 8, 52,
	2, 7, 54,
	// C
	1, 0, 0,
	0, 4, 0,
	1, 4, 0,
	1, 5, 1,
	0, 5, 1,
	0, 2, 2,
	2, 3, 2,
	0, 3, 2,
	0, 6, 3,
	2, 6, 3,
	4, 3, 4,
	5, 6, 5,
	6, 7, 6,
	2, 7, 6,
	6, 8, 7,
	7, 8, 7,
	6, 9, 8,
	9, 8, 8,
	5, 9, 9,
	5, 10, 10,
	9, 10, 10,
	4, 8, 11,
	4, 9, 11,
	11, 9, 12,
	11, 10, 12,
	11, 4, 13,
	12, 11, 14,
	12, 10, 14,
	13, 11, 15,
	13, 12, 15,
	1, 11, 16,
	1, 13, 16,
	12, 15, 18,
	13, 15, 18,
	13, 14, 19,
	1, 14, 19,
	13, 16, 20,
	16, 14, 20,
	15, 16, 21,
	18, 17, 22,
	17, 16, 22,
	18, 16, 22,
	17, 14, 23,
	19, 18, 24,
	19, 16, 24,
	15, 19, 25,
	20, 18, 26,
	20, 19, 26,
	15, 20, 27,
	21, 20, 28,
	21, 15, 28,
	21, 18, 29,
	21, 17, 30,
	21, 12, 31,
	12, 17, 31,
	10, 14, 33,
	10, 17, 33,
	5, 14, 35,
	3, 7, 37,
	3, 8, 37,
};

const uint8_t font_3d_simple_edge_flags[] = {
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
	// B instance 0
	12,
	14,
	14,
	14,
	14,
	14,
	12,
	12,
	12,
	12,
	12,
	14,
	12,
	14,
	14,
	14,
	14,
	12,
	12,
	12,
	14,
	12,
	14,
	14,
	14,
	14,
	12,
	12,
	12,
	12,
	14,
	12,
	14,
	12,
	14,
	12,
	14,
	14,
	14,
	12,
	12,
	12,
	12,
	14,
	14,
	12,
	12,
	14,
	14,
	14,
	12,
	12,
	12,
	12,
	14,
	14,
	14,
	12,
	12,
	12,
	12,
	12,
	14,
	14,
	14,
	12,
	14,
	12,
	14,
	12,
	14,
	12,
	14,
	12,
	14,
	12,
	12,
	14,
	12,
	12,
	12,
	14,
	12,
	12,
	// C instance 0
	14,
	12,
	12,
	14,
	12,
	14,
	14,
	12,
	14,
	12,
	14,
	14,
	14,
	14,
	12,
	14,
	12,
	14,
	12,
	12,
	14,
	12,
	14,
	12,
	14,
	14,
	14,
	12,
	12,
	12,
	12,
	14,
	12,
	14,
	14,
	12,
	12,
	14,
	14,
	14,
	12,
	12,
	12,
	14,
	14,
	12,
	12,
	14,
	14,
	14,
	12,
	14,
	12,
	14,
	14,
	12,
	14,
	14,
	12,
	14,
};

l3d_font_descriptor_t font_3d_simple = {
	.char_count = FONT_3D_SIMPLE_MESH_COUNT,
	.model_vert_counts = (uint16_t *)font_3d_simple_model_vert_counts,
	.model_face_counts = (uint16_t *)font_3d_simple_model_face_counts,
	.model_edge_counts = (uint16_t *)font_3d_simple_model_edge_counts,
	.model_vertex_data = (int8_t *)font_3d_simple_model_vertex_data,
	.model_face_data = (uint16_t *)font_3d_simple_model_face_data,
	.model_edge_data = (uint16_t *)font_3d_simple_model_edge_data,
	.edges_flags = (uint8_t *)font_3d_simple_edge_flags,
	.total_edge_count = (uint16_t)FONT_3D_SIMPLE_MODEL_EDGE_COUNT,
};

// l3d_scene_t font_3d_simple;
// l3d_vec4_t font_3d_simple_vertices_world[FONT_3D_SIMPLE_TRANSFORMED_VERT_COUNT];
// l3d_vec4_t font_3d_simple_vertices_projected[FONT_3D_SIMPLE_TRANSFORMED_VERT_COUNT];
// uint8_t font_3d_simple_face_flags[FONT_3D_SIMPLE_FACE_FLAG_COUNT];
// l3d_obj3d_t font_3d_simple_objects[FONT_3D_SIMPLE_OBJ_COUNT];
// l3d_scene_instance_desc_t font_3d_simple_mesh_instances[FONT_3D_SIMPLE_MESH_COUNT];
// l3d_camera_t font_3d_simple_cameras[FONT_3D_SIMPLE_CAM_COUNT];

// static l3d_err_t init_objects(void) {
// 	// A
// 	for (uint16_t i = font_3d_simple_mesh_instances[0].first_instance_idx;
// 		i < font_3d_simple_mesh_instances[0].first_instance_idx + FONT_3D_SIMPLE_OBJ_A_INSTANCE_COUNT;
// 		i++) {
// 		font_3d_simple_objects[i].mesh.vert_count = MESH_A_VERT_COUNT;
// 		font_3d_simple_objects[i].mesh.tri_count = MESH_A_FACE_COUNT;
// 		font_3d_simple_objects[i].mesh.edge_count = MESH_A_EDGE_COUNT;
// 	}

// 	// B
// 	for (uint16_t i = font_3d_simple_mesh_instances[1].first_instance_idx;
// 		i < font_3d_simple_mesh_instances[1].first_instance_idx + FONT_3D_SIMPLE_OBJ_B_INSTANCE_COUNT;
// 		i++) {
// 		font_3d_simple_objects[i].mesh.vert_count = MESH_B_VERT_COUNT;
// 		font_3d_simple_objects[i].mesh.tri_count = MESH_B_FACE_COUNT;
// 		font_3d_simple_objects[i].mesh.edge_count = MESH_B_EDGE_COUNT;
// 	}

// 	// C
// 	for (uint16_t i = font_3d_simple_mesh_instances[2].first_instance_idx;
// 		i < font_3d_simple_mesh_instances[2].first_instance_idx + FONT_3D_SIMPLE_OBJ_C_INSTANCE_COUNT;
// 		i++) {
// 		font_3d_simple_objects[i].mesh.vert_count = MESH_C_VERT_COUNT;
// 		font_3d_simple_objects[i].mesh.tri_count = MESH_C_FACE_COUNT;
// 		font_3d_simple_objects[i].mesh.edge_count = MESH_C_EDGE_COUNT;
// 	}


// 	uint16_t model_vert_data_offset = 0;
// 	uint16_t model_tri_data_offset = 0;
// 	uint16_t model_edge_data_offset = 0;
// 	uint16_t transformed_vertices_offset = 0;
// 	uint16_t tris_flags_offset = 0;
// 	uint16_t edges_flags_offset = 0;

// 	for (uint16_t i = 0; i < FONT_3D_SIMPLE_MESH_COUNT; i++) {
// 		for (uint16_t instance_idx = 0; instance_idx < font_3d_simple_mesh_instances[i].instance_count; instance_idx++) {
// 			uint16_t obj_id = font_3d_simple_mesh_instances[i].first_instance_idx + instance_idx;
// 			font_3d_simple_objects[obj_id].mesh.model_vert_data_offset = model_vert_data_offset;
// 			font_3d_simple_objects[obj_id].mesh.model_tri_data_offset = model_tri_data_offset;
// 			font_3d_simple_objects[obj_id].mesh.model_edge_data_offset = model_edge_data_offset;

// 			font_3d_simple_objects[obj_id].mesh.transformed_vertices_offset = transformed_vertices_offset;
// 			font_3d_simple_objects[obj_id].mesh.tris_flags_offset = tris_flags_offset;	// not used for now
// 			font_3d_simple_objects[obj_id].mesh.edges_flags_offset = edges_flags_offset;

// 			transformed_vertices_offset += font_3d_simple_objects[obj_id].mesh.vert_count;
// 			tris_flags_offset += font_3d_simple_objects[obj_id].mesh.tri_count;
// 			edges_flags_offset += font_3d_simple_objects[obj_id].mesh.edge_count;
// 		}

// 		// Update offsets
// 		model_vert_data_offset += font_3d_simple_objects[font_3d_simple_mesh_instances[i].first_instance_idx].mesh.vert_count * 3; // check correctness
// 		model_tri_data_offset += font_3d_simple_objects[font_3d_simple_mesh_instances[i].first_instance_idx].mesh.tri_count * 3; // check correctness
// 		model_edge_data_offset += font_3d_simple_objects[font_3d_simple_mesh_instances[i].first_instance_idx].mesh.edge_count * 3; // check correctness
// 	}

// 	// Common for all objects
// 	for (uint16_t obj_id = 0; obj_id < FONT_3D_SIMPLE_OBJ_COUNT; obj_id++) {
// 		font_3d_simple_objects[obj_id].local_pos = l3d_getZeroVec4();
// 		font_3d_simple_objects[obj_id].orientation = l3d_getIdentityQuat();
// 		// font_3d_simple_objects[obj_id].wireframe_colour.value = L3D_COLOUR_WHITE;
// 		font_3d_simple_objects[obj_id].wireframe_colour = L3D_COLOUR_WHITE;

// 		// Local orientation unit vectors
// 		font_3d_simple_objects[obj_id].u[0] = l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);
// 		font_3d_simple_objects[obj_id].u[1] = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
// 		font_3d_simple_objects[obj_id].u[2] = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
// 		font_3d_simple_objects[obj_id].u[3] = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
// 		// (parent, children, group, etc) to be added...
// 	}

// 	return L3D_OK;
// }

// static l3d_err_t init_cameras(void) {
// 	l3d_err_t ret = L3D_OK;
// 	for (uint16_t i=0; i<FONT_3D_SIMPLE_CAM_COUNT; i++){
// 		ret = l3d_cam_reset(&font_3d_simple.cameras[i]);

// 		if (ret != L3D_OK)
// 			return ret;
// 	}

// 	return ret;
// }

// l3d_err_t font_3d_simple_init(void) {
	// font_3d_simple.model_vert_data = font_3d_simple_model_vertex_data;
	// font_3d_simple.model_tri_data = font_3d_simple_model_face_data;
	// font_3d_simple.model_edge_data = font_3d_simple_model_edge_data;
	
	// font_3d_simple.model_vertex_count = FONT_3D_SIMPLE_MODEL_VERT_COUNT;
	// font_3d_simple.model_tri_count = FONT_3D_SIMPLE_MODEL_FACE_COUNT;
	// font_3d_simple.model_edge_count = FONT_3D_SIMPLE_MODEL_EDGE_COUNT;
	
	// font_3d_simple.vertices_world = font_3d_simple_vertices_world;
	// font_3d_simple.vertices_projected = font_3d_simple_vertices_projected;
	
	// font_3d_simple.tri_flags = font_3d_simple_face_flags;
	// font_3d_simple.edge_flags = font_3d_simple_edge_flags;
	
	// font_3d_simple.transformed_vertex_count = FONT_3D_SIMPLE_TRANSFORMED_VERT_COUNT;
	// font_3d_simple.tri_flag_count = FONT_3D_SIMPLE_FACE_FLAG_COUNT;
	// font_3d_simple.edge_flag_count = FONT_3D_SIMPLE_EDGE_FLAG_COUNT;
	
	// font_3d_simple.objects = font_3d_simple_objects;
	// font_3d_simple.object_count = FONT_3D_SIMPLE_OBJ_COUNT;
	
	// // A
	// font_3d_simple_mesh_instances[0].first_instance_idx = 0;
	// font_3d_simple_mesh_instances[0].instance_count = FONT_3D_SIMPLE_OBJ_A_INSTANCE_COUNT;
	// // B
	// font_3d_simple_mesh_instances[1].first_instance_idx = 1;
	// font_3d_simple_mesh_instances[1].instance_count = FONT_3D_SIMPLE_OBJ_B_INSTANCE_COUNT;
	// // C
	// font_3d_simple_mesh_instances[2].first_instance_idx = 2;
	// font_3d_simple_mesh_instances[2].instance_count = FONT_3D_SIMPLE_OBJ_C_INSTANCE_COUNT;
	
	// font_3d_simple.cameras = font_3d_simple_cameras;
	// font_3d_simple.camera_count = FONT_3D_SIMPLE_CAM_COUNT;
	
	// font_3d_simple.active_camera_idx = 0;
	
	// l3d_err_t ret = init_objects();

	// if (ret != L3D_OK)
	// 	return ret;

	// ret = init_cameras();

	// if (ret != L3D_OK)
	// 	return ret;
	
	// l3d_makeProjectionMatrix(&font_3d_simple.mat_proj, l3d_scene_getActiveCamera(&font_3d_simple));
	// l3d_computeViewMatrix(l3d_scene_getActiveCamera(&font_3d_simple), &(font_3d_simple.mat_view));
// 	return l3d_setupObjects(&font_3d_simple);
// }

