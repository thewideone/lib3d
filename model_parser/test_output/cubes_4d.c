#include "cubes_4d.h"
#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_core.h" // for l3d_setupObjects()

// 
// Scene defines
// 
#define CUBES_4D_MODEL_VERT_COUNT 32
#define CUBES_4D_MODEL_FACE_COUNT 24
#define CUBES_4D_MODEL_EDGE_COUNT 76

// WIP: Calculated by scene-model parser (or however I'm gonna call the script),
// = sum[for each object (model_vertex_count * no_of_object_instances)]
#define CUBES_4D_TRANSFORMED_VERT_COUNT 32
#define CUBES_4D_FACE_FLAG_COUNT 24
#define CUBES_4D_EDGE_FLAG_COUNT 76

// 
// Object defines
// 
#define MESH_CUBE4D_NO_FACES_VERT_COUNT 16
#define MESH_CUBE4D_NO_FACES_FACE_COUNT 0
#define MESH_CUBE4D_NO_FACES_EDGE_COUNT 32
#define MESH_CUBE4D_WITH_FACES_VERT_COUNT 16
#define MESH_CUBE4D_WITH_FACES_FACE_COUNT 24
#define MESH_CUBE4D_WITH_FACES_EDGE_COUNT 44

const l3d_flp_t cubes_4d_model_vertex_data[] = {
	// cube4d_no_faces
	1.000000, 1.000000, -1.000000, 
	1.000000, -1.000000, -1.000000, 
	1.000000, 1.000000, 1.000000, 
	1.000000, -1.000000, 1.000000, 
	-1.000000, 1.000000, -1.000000, 
	-1.000000, -1.000000, -1.000000, 
	-1.000000, 1.000000, 1.000000, 
	-1.000000, -1.000000, 1.000000, 
	-1.867836, -1.867836, 1.867836, 
	-1.867836, 1.867836, 1.867836, 
	-1.867836, -1.867836, -1.867836, 
	-1.867836, 1.867836, -1.867836, 
	1.867836, -1.867836, 1.867836, 
	1.867836, 1.867836, 1.867836, 
	1.867836, -1.867836, -1.867836, 
	1.867836, 1.867836, -1.867836, 
	// cube4d_with_faces
	1.000000, 1.000000, -1.000000, 
	1.000000, -1.000000, -1.000000, 
	1.000000, 1.000000, 1.000000, 
	1.000000, -1.000000, 1.000000, 
	-1.000000, 1.000000, -1.000000, 
	-1.000000, -1.000000, -1.000000, 
	-1.000000, 1.000000, 1.000000, 
	-1.000000, -1.000000, 1.000000, 
	-1.867836, -1.867836, 1.867836, 
	-1.867836, 1.867836, 1.867836, 
	-1.867836, -1.867836, -1.867836, 
	-1.867836, 1.867836, -1.867836, 
	1.867836, -1.867836, 1.867836, 
	1.867836, 1.867836, 1.867836, 
	1.867836, -1.867836, -1.867836, 
	1.867836, 1.867836, -1.867836, 
};

const uint16_t cubes_4d_model_face_data[] = {
	// cube4d_no_faces
	// cube4d_with_faces
	4, 2, 0, 
	2, 7, 3, 
	6, 5, 7, 
	1, 7, 5, 
	0, 3, 1, 
	4, 1, 5, 
	9, 10, 8, 
	11, 14, 10, 
	15, 12, 14, 
	13, 8, 12, 
	14, 8, 10, 
	11, 13, 15, 
	4, 6, 2, 
	2, 6, 7, 
	6, 4, 5, 
	1, 3, 7, 
	0, 2, 3, 
	4, 0, 1, 
	9, 11, 10, 
	11, 15, 14, 
	15, 13, 12, 
	13, 9, 8, 
	14, 12, 8, 
	11, 9, 13, 
};

const uint16_t cubes_4d_model_edge_data[] = {
	// cube4d_no_faces
	5, 7, 0,
	1, 5, 0,
	0, 1, 0,
	7, 6, 0,
	2, 3, 0,
	4, 5, 0,
	2, 6, 0,
	0, 2, 0,
	7, 3, 0,
	6, 4, 0,
	4, 0, 0,
	3, 1, 0,
	10, 8, 0,
	8, 9, 0,
	9, 11, 0,
	11, 10, 0,
	14, 10, 0,
	11, 15, 0,
	15, 14, 0,
	12, 14, 0,
	15, 13, 0,
	13, 12, 0,
	8, 12, 0,
	13, 9, 0,
	4, 11, 0,
	0, 15, 0,
	1, 14, 0,
	5, 10, 0,
	7, 8, 0,
	6, 9, 0,
	2, 13, 0,
	3, 12, 0,
	// cube4d_with_faces
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
	9, 10, 6,
	10, 8, 6,
	9, 8, 6,
	11, 14, 7,
	14, 10, 7,
	11, 10, 7,
	15, 12, 8,
	12, 14, 8,
	15, 14, 8,
	13, 8, 9,
	8, 12, 9,
	13, 12, 9,
	14, 8, 10,
	11, 13, 11,
	13, 15, 11,
	11, 15, 11,
	4, 6, 12,
	6, 2, 12,
	9, 11, 18,
	13, 9, 21,
	4, 11, 0,
	0, 15, 0,
	1, 14, 0,
	5, 10, 0,
	7, 8, 0,
	6, 9, 0,
	2, 13, 0,
	3, 12, 0,
};

uint8_t cubes_4d_edge_flags[] = {
	// cube4d_no_faces instance 0
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	// cube4d_with_faces instance 0
	12,
	14,
	14,
	12,
	14,
	14,
	12,
	14,
	14,
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
	12,
	14,
	14,
	12,
	14,
	14,
	12,
	14,
	14,
	12,
	12,
	14,
	14,
	14,
	14,
	14,
	14,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
	6,
};


l3d_scene_t cubes_4d;
l3d_vec4_t cubes_4d_vertices_world[CUBES_4D_TRANSFORMED_VERT_COUNT];
l3d_vec4_t cubes_4d_vertices_projected[CUBES_4D_TRANSFORMED_VERT_COUNT];
uint8_t cubes_4d_face_flags[CUBES_4D_FACE_FLAG_COUNT];
l3d_obj3d_t cubes_4d_objects[CUBES_4D_OBJ_COUNT];
l3d_scene_instance_desc_t cubes_4d_mesh_instances[CUBES_4D_MESH_COUNT];
l3d_camera_t cubes_4d_cameras[CUBES_4D_CAM_COUNT];

static l3d_err_t init_objects(void) {
	// cube4d_no_faces
	for (uint16_t i = cubes_4d_mesh_instances[0].first_instance_idx;
		i < cubes_4d_mesh_instances[0].first_instance_idx + CUBES_4D_OBJ_CUBE4D_NO_FACES_INSTANCE_COUNT;
		i++) {
		cubes_4d_objects[i].mesh.vert_count = MESH_CUBE4D_NO_FACES_VERT_COUNT;
		cubes_4d_objects[i].mesh.tri_count = MESH_CUBE4D_NO_FACES_FACE_COUNT;
		cubes_4d_objects[i].mesh.edge_count = MESH_CUBE4D_NO_FACES_EDGE_COUNT;
	}

	// cube4d_with_faces
	for (uint16_t i = cubes_4d_mesh_instances[1].first_instance_idx;
		i < cubes_4d_mesh_instances[1].first_instance_idx + CUBES_4D_OBJ_CUBE4D_WITH_FACES_INSTANCE_COUNT;
		i++) {
		cubes_4d_objects[i].mesh.vert_count = MESH_CUBE4D_WITH_FACES_VERT_COUNT;
		cubes_4d_objects[i].mesh.tri_count = MESH_CUBE4D_WITH_FACES_FACE_COUNT;
		cubes_4d_objects[i].mesh.edge_count = MESH_CUBE4D_WITH_FACES_EDGE_COUNT;
	}


	uint16_t model_vert_data_offset = 0;
	uint16_t model_tri_data_offset = 0;
	uint16_t model_edge_data_offset = 0;
	uint16_t transformed_vertices_offset = 0;
	uint16_t tris_flags_offset = 0;
	uint16_t edges_flags_offset = 0;

	for (uint16_t i = 0; i < CUBES_4D_MESH_COUNT; i++) {
		for (uint16_t instance_idx = 0; instance_idx < cubes_4d_mesh_instances[i].instance_count; instance_idx++) {
			uint16_t obj_id = cubes_4d_mesh_instances[i].first_instance_idx + instance_idx;
			cubes_4d_objects[obj_id].mesh.model_vert_data_offset = model_vert_data_offset;
			cubes_4d_objects[obj_id].mesh.model_tri_data_offset = model_tri_data_offset;
			cubes_4d_objects[obj_id].mesh.model_edge_data_offset = model_edge_data_offset;

			cubes_4d_objects[obj_id].mesh.transformed_vertices_offset = transformed_vertices_offset;
			cubes_4d_objects[obj_id].mesh.tris_flags_offset = tris_flags_offset;	// not used for now
			cubes_4d_objects[obj_id].mesh.edges_flags_offset = edges_flags_offset;

			transformed_vertices_offset += cubes_4d_objects[obj_id].mesh.vert_count;
			tris_flags_offset += cubes_4d_objects[obj_id].mesh.tri_count;
			edges_flags_offset += cubes_4d_objects[obj_id].mesh.edge_count;
		}

		// Update offsets
		model_vert_data_offset += cubes_4d_objects[cubes_4d_mesh_instances[i].first_instance_idx].mesh.vert_count * 3; // check correctness
		model_tri_data_offset += cubes_4d_objects[cubes_4d_mesh_instances[i].first_instance_idx].mesh.tri_count * 3; // check correctness
		model_edge_data_offset += cubes_4d_objects[cubes_4d_mesh_instances[i].first_instance_idx].mesh.edge_count * 3; // check correctness
	}

	// Common for all objects
	for (uint16_t obj_id = 0; obj_id < CUBES_4D_OBJ_COUNT; obj_id++) {
		cubes_4d_objects[obj_id].local_pos = l3d_getZeroVec4();
		cubes_4d_objects[obj_id].orientation = l3d_getIdentityQuat();
		// cubes_4d_objects[obj_id].wireframe_colour.value = L3D_COLOUR_WHITE;
		cubes_4d_objects[obj_id].wireframe_colour = L3D_COLOUR_WHITE;

		// Local orientation unit vectors
		cubes_4d_objects[obj_id].u[0] = l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);
		cubes_4d_objects[obj_id].u[1] = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
		cubes_4d_objects[obj_id].u[2] = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
		cubes_4d_objects[obj_id].u[3] = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
		// (parent, children, group, etc) to be added...
	}

	return L3D_OK;
}

static l3d_err_t init_cameras(void) {
	l3d_err_t ret = L3D_OK;
	for (uint16_t i=0; i<CUBES_4D_CAM_COUNT; i++){
		ret = l3d_cam_reset(&cubes_4d.cameras[i]);

		if (ret != L3D_OK)
			return ret;
	}

	return ret;
}

l3d_err_t cubes_4d_init(void) {
	cubes_4d.model_vert_data = cubes_4d_model_vertex_data;
	cubes_4d.model_tri_data = cubes_4d_model_face_data;
	cubes_4d.model_edge_data = cubes_4d_model_edge_data;
	
	cubes_4d.model_vertex_count = CUBES_4D_MODEL_VERT_COUNT;
	cubes_4d.model_tri_count = CUBES_4D_MODEL_FACE_COUNT;
	cubes_4d.model_edge_count = CUBES_4D_MODEL_EDGE_COUNT;
	
	cubes_4d.vertices_world = cubes_4d_vertices_world;
	cubes_4d.vertices_projected = cubes_4d_vertices_projected;
	
	cubes_4d.tri_flags = cubes_4d_face_flags;
	cubes_4d.edge_flags = cubes_4d_edge_flags;
	
	cubes_4d.transformed_vertex_count = CUBES_4D_TRANSFORMED_VERT_COUNT;
	cubes_4d.tri_flag_count = CUBES_4D_FACE_FLAG_COUNT;
	cubes_4d.edge_flag_count = CUBES_4D_EDGE_FLAG_COUNT;
	
	cubes_4d.objects = cubes_4d_objects;
	cubes_4d.object_count = CUBES_4D_OBJ_COUNT;
	
	// cube4d_no_faces
	cubes_4d_mesh_instances[0].first_instance_idx = 0;
	cubes_4d_mesh_instances[0].instance_count = CUBES_4D_OBJ_CUBE4D_NO_FACES_INSTANCE_COUNT;
	// cube4d_with_faces
	cubes_4d_mesh_instances[1].first_instance_idx = 1;
	cubes_4d_mesh_instances[1].instance_count = CUBES_4D_OBJ_CUBE4D_WITH_FACES_INSTANCE_COUNT;
	
	cubes_4d.cameras = cubes_4d_cameras;
	cubes_4d.camera_count = CUBES_4D_CAM_COUNT;
	
	cubes_4d.active_camera_idx = 0;
	
	l3d_err_t ret = init_objects();

	if (ret != L3D_OK)
		return ret;

	ret = init_cameras();

	if (ret != L3D_OK)
		return ret;
	
	l3d_makeProjectionMatrix(&cubes_4d.mat_proj, l3d_scene_getActiveCamera(&cubes_4d));
	l3d_computeViewMatrix(l3d_scene_getActiveCamera(&cubes_4d), &(cubes_4d.mat_view));
	return l3d_setupObjects(&cubes_4d);
}

