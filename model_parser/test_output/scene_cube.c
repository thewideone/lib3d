#include "scene_cube.h"
#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_core.h // for l3d_setupObjects()"

// 
// Scene defines
// 
#define SCENE_CUBE_MODEL_VERT_COUNT 13
#define SCENE_CUBE_MODEL_FACE_COUNT 18
#define SCENE_CUBE_MODEL_EDGE_COUNT 27

// WIP: Calculated by scene-model parser (or however I'm gonna call the script),
// = sum[for each object (model_vertex_count * no_of_object_instances)]
#define SCENE_CUBE_TRANSFORMED_VERT_COUNT 44
#define SCENE_CUBE_FACE_FLAG_COUNT 60
#define SCENE_CUBE_EDGE_FLAG_COUNT 90

// 
// Object defines
// 
#define MESH_CUBE_TRI_VERT_COUNT 8
#define MESH_CUBE_TRI_FACE_COUNT 12
#define MESH_CUBE_TRI_EDGE_COUNT 18
#define MESH_PYRAMID_TRI_VERT_COUNT 5
#define MESH_PYRAMID_TRI_FACE_COUNT 6
#define MESH_PYRAMID_TRI_EDGE_COUNT 9

const l3d_fxp_t scene_cube_model_vertex_data[] = {
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

const uint16_t scene_cube_model_face_data[] = {
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
	1, 2, 4, 
	4, 2, 3, 
	0, 4, 3, 
	0, 2, 1, 
	3, 2, 0, 
	0, 1, 4, 
};

const uint16_t scene_cube_model_edge_data[] = {
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
	1, 2, 0,
	2, 4, 0,
	1, 4, 0,
	2, 3, 1,
	4, 3, 1,
	0, 4, 2,
	0, 3, 2,
	0, 2, 3,
	0, 1, 3,
};

uint8_t scene_cube_edge_flags[] = {
	// cube_tri instance 0
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
	// cube_tri instance 1
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
	// cube_tri instance 2
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
	// pyramid_tri instance 0
	6,
	6,
	6,
	6,
	6,
	4,
	6,
	6,
	6,
	// pyramid_tri instance 1
	6,
	6,
	6,
	6,
	6,
	4,
	6,
	6,
	6,
	// pyramid_tri instance 2
	6,
	6,
	6,
	6,
	6,
	4,
	6,
	6,
	6,
	// pyramid_tri instance 3
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


l3d_scene_t scene_cube;
l3d_vec4_t scene_cube_vertices_world[SCENE_CUBE_TRANSFORMED_VERT_COUNT];
l3d_vec4_t scene_cube_vertices_projected[SCENE_CUBE_TRANSFORMED_VERT_COUNT];
uint8_t scene_cube_face_flags[SCENE_CUBE_FACE_FLAG_COUNT];
l3d_obj3d_t scene_cube_objects[SCENE_CUBE_OBJ_COUNT];
l3d_scene_instance_desc_t scene_cube_mesh_instances[SCENE_CUBE_MESH_COUNT];
l3d_camera_t scene_cube_cameras[SCENE_CUBE_CAM_COUNT];

static l3d_err_t init_objects(void) {
	// cube_tri
	for (uint16_t i = scene_cube_mesh_instances[0].first_instance_idx;
		i < scene_cube_mesh_instances[0].first_instance_idx + SCENE_CUBE_OBJ_CUBE_TRI_INSTANCE_COUNT;
		i++) {
		scene_cube_objects[i].mesh.vert_count = MESH_CUBE_TRI_VERT_COUNT;
		scene_cube_objects[i].mesh.tri_count = MESH_CUBE_TRI_FACE_COUNT;
		scene_cube_objects[i].mesh.edge_count = MESH_CUBE_TRI_EDGE_COUNT;
	}

	// pyramid_tri
	for (uint16_t i = scene_cube_mesh_instances[1].first_instance_idx;
		i < scene_cube_mesh_instances[1].first_instance_idx + SCENE_CUBE_OBJ_PYRAMID_TRI_INSTANCE_COUNT;
		i++) {
		scene_cube_objects[i].mesh.vert_count = MESH_PYRAMID_TRI_VERT_COUNT;
		scene_cube_objects[i].mesh.tri_count = MESH_PYRAMID_TRI_FACE_COUNT;
		scene_cube_objects[i].mesh.edge_count = MESH_PYRAMID_TRI_EDGE_COUNT;
	}


	uint16_t model_vert_data_offset = 0;
	uint16_t model_tri_data_offset = 0;
	uint16_t model_edge_data_offset = 0;
	uint16_t transformed_vertices_offset = 0;
	uint16_t tris_flags_offset = 0;
	uint16_t edges_flags_offset = 0;

	for (uint16_t i = 0; i < SCENE_CUBE_MESH_COUNT; i++) {
		for (uint16_t instance_idx = 0; instance_idx < scene_cube_mesh_instances[i].instance_count; instance_idx++) {
			uint16_t obj_id = scene_cube_mesh_instances[i].first_instance_idx + instance_idx;
			scene_cube_objects[obj_id].mesh.model_vert_data_offset = model_vert_data_offset;
			scene_cube_objects[obj_id].mesh.model_tri_data_offset = model_tri_data_offset;
			scene_cube_objects[obj_id].mesh.model_edge_data_offset = model_edge_data_offset;

			scene_cube_objects[obj_id].mesh.transformed_vertices_offset = transformed_vertices_offset;
			scene_cube_objects[obj_id].mesh.tris_flags_offset = tris_flags_offset;	// not used for now
			scene_cube_objects[obj_id].mesh.edges_flags_offset = edges_flags_offset;

			transformed_vertices_offset += scene_cube_objects[obj_id].mesh.vert_count;
			tris_flags_offset += scene_cube_objects[obj_id].mesh.tri_count;
			edges_flags_offset += scene_cube_objects[obj_id].mesh.edge_count;
		}

		// Update offsets
		model_vert_data_offset += scene_cube_objects[scene_cube_mesh_instances[i].first_instance_idx].mesh.vert_count * 3; // check correctness
		model_tri_data_offset += scene_cube_objects[scene_cube_mesh_instances[i].first_instance_idx].mesh.tri_count * 3; // check correctness
		model_edge_data_offset += scene_cube_objects[scene_cube_mesh_instances[i].first_instance_idx].mesh.edge_count * 3; // check correctness
	}

	// Common for all objects
	for (uint16_t obj_id = 0; obj_id < SCENE_CUBE_OBJ_COUNT; obj_id++) {
		scene_cube_objects[obj_id].local_pos = l3d_getZeroVec4();
		scene_cube_objects[obj_id].orientation = l3d_getIdentityQuat();
		// scene_cube_objects[obj_id].wireframe_colour.value = L3D_COLOUR_WHITE;
		scene_cube_objects[obj_id].wireframe_colour = L3D_COLOUR_WHITE;

		// Local orientation unit vectors
		scene_cube_objects[obj_id].u[0] = l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);
		scene_cube_objects[obj_id].u[1] = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
		scene_cube_objects[obj_id].u[2] = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
		scene_cube_objects[obj_id].u[3] = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
		// (parent, children, group, etc) to be added...
	}

	return L3D_OK;
}

static l3d_err_t init_cameras(void) {
	l3d_err_t ret = L3D_OK;
	for (uint16_t i=0; i<SCENE_CUBE_CAM_COUNT; i++){
		ret = l3d_cam_reset(&scene_cube.cameras[i]);

		if (ret != L3D_OK)
			return ret;
	}

	return ret;
}

l3d_err_t scene_cube_init(void) {
	scene_cube.model_vert_data = scene_cube_model_vertex_data;
	scene_cube.model_tri_data = scene_cube_model_face_data;
	scene_cube.model_edge_data = scene_cube_model_edge_data;
	
	scene_cube.model_vertex_count = SCENE_CUBE_MODEL_VERT_COUNT;
	scene_cube.model_tri_count = SCENE_CUBE_MODEL_FACE_COUNT;
	scene_cube.model_edge_count = SCENE_CUBE_MODEL_EDGE_COUNT;
	
	scene_cube.vertices_world = scene_cube_vertices_world;
	scene_cube.vertices_projected = scene_cube_vertices_projected;
	
	scene_cube.tri_flags = scene_cube_face_flags;
	scene_cube.edge_flags = scene_cube_edge_flags;
	
	scene_cube.transformed_vertex_count = SCENE_CUBE_TRANSFORMED_VERT_COUNT;
	scene_cube.tri_flag_count = SCENE_CUBE_FACE_FLAG_COUNT;
	scene_cube.edge_flag_count = SCENE_CUBE_EDGE_FLAG_COUNT;
	
	scene_cube.objects = scene_cube_objects;
	scene_cube.object_count = SCENE_CUBE_OBJ_COUNT;
	
	// cube_tri
	scene_cube_mesh_instances[0].first_instance_idx = 0;
	scene_cube_mesh_instances[0].instance_count = SCENE_CUBE_OBJ_CUBE_TRI_INSTANCE_COUNT;
	// pyramid_tri
	scene_cube_mesh_instances[1].first_instance_idx = 3;
	scene_cube_mesh_instances[1].instance_count = SCENE_CUBE_OBJ_PYRAMID_TRI_INSTANCE_COUNT;
	
	scene_cube.cameras = scene_cube_cameras;
	scene_cube.camera_count = SCENE_CUBE_CAM_COUNT;
	
	scene_cube.active_camera = &scene_cube.cameras[0];
	
	l3d_err_t ret = init_objects();

	if (ret != L3D_OK)
		return ret;

	ret = init_cameras();

	if (ret != L3D_OK)
		return ret;
	
	l3d_makeProjectionMatrix(&scene_cube.mat_proj, scene_cube.active_camera);
	l3d_computeViewMatrix(scene_cube.active_camera, &(scene_cube.mat_view));
	return l3d_setupObjects(&scene_cube);
}

