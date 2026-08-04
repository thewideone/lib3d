#include "scene.h"
#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_core.h" // for l3d_setupObjects()

// 
// Scene defines
// 
#define SCENE_MODEL_VERT_COUNT 16
#define SCENE_MODEL_FACE_COUNT 0
#define SCENE_MODEL_EDGE_COUNT 0

// WIP: Calculated by scene-model parser (or however I'm gonna call the script),
// = sum[for each object (model_vertex_count * no_of_object_instances)]
#define SCENE_TRANSFORMED_VERT_COUNT 16
#define SCENE_FACE_FLAG_COUNT 0
#define SCENE_EDGE_FLAG_COUNT 0

// 
// Object defines
// 
#define MESH_CUBE4D_NO_FACES_VERT_COUNT 16
#define MESH_CUBE4D_NO_FACES_FACE_COUNT 0
#define MESH_CUBE4D_NO_FACES_EDGE_COUNT 0

const l3d_flp_t scene_model_vertex_data[] = {
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
};

const uint16_t scene_model_face_data[] = {
	// cube4d_no_faces
};

const uint16_t scene_model_edge_data[] = {
	// cube4d_no_faces
};

uint8_t scene_edge_flags[] = {
	// cube4d_no_faces instance 0
};


l3d_scene_t scene;
l3d_vec4_t scene_vertices_world[SCENE_TRANSFORMED_VERT_COUNT];
l3d_vec4_t scene_vertices_projected[SCENE_TRANSFORMED_VERT_COUNT];
uint8_t scene_face_flags[SCENE_FACE_FLAG_COUNT];
l3d_obj3d_t scene_objects[SCENE_OBJ_COUNT];
l3d_scene_instance_desc_t scene_mesh_instances[SCENE_MESH_COUNT];
l3d_camera_t scene_cameras[SCENE_CAM_COUNT];

static l3d_err_t init_objects(void) {
	// cube4d_no_faces
	for (uint16_t i = scene_mesh_instances[0].first_instance_idx;
		i < scene_mesh_instances[0].first_instance_idx + SCENE_OBJ_CUBE4D_NO_FACES_INSTANCE_COUNT;
		i++) {
		scene_objects[i].mesh.vert_count = MESH_CUBE4D_NO_FACES_VERT_COUNT;
		scene_objects[i].mesh.tri_count = MESH_CUBE4D_NO_FACES_FACE_COUNT;
		scene_objects[i].mesh.edge_count = MESH_CUBE4D_NO_FACES_EDGE_COUNT;
	}


	uint16_t model_vert_data_offset = 0;
	uint16_t model_tri_data_offset = 0;
	uint16_t model_edge_data_offset = 0;
	uint16_t transformed_vertices_offset = 0;
	uint16_t tris_flags_offset = 0;
	uint16_t edges_flags_offset = 0;

	for (uint16_t i = 0; i < SCENE_MESH_COUNT; i++) {
		for (uint16_t instance_idx = 0; instance_idx < scene_mesh_instances[i].instance_count; instance_idx++) {
			uint16_t obj_id = scene_mesh_instances[i].first_instance_idx + instance_idx;
			scene_objects[obj_id].mesh.model_vert_data_offset = model_vert_data_offset;
			scene_objects[obj_id].mesh.model_tri_data_offset = model_tri_data_offset;
			scene_objects[obj_id].mesh.model_edge_data_offset = model_edge_data_offset;

			scene_objects[obj_id].mesh.transformed_vertices_offset = transformed_vertices_offset;
			scene_objects[obj_id].mesh.tris_flags_offset = tris_flags_offset;	// not used for now
			scene_objects[obj_id].mesh.edges_flags_offset = edges_flags_offset;

			transformed_vertices_offset += scene_objects[obj_id].mesh.vert_count;
			tris_flags_offset += scene_objects[obj_id].mesh.tri_count;
			edges_flags_offset += scene_objects[obj_id].mesh.edge_count;
		}

		// Update offsets
		model_vert_data_offset += scene_objects[scene_mesh_instances[i].first_instance_idx].mesh.vert_count * 3; // check correctness
		model_tri_data_offset += scene_objects[scene_mesh_instances[i].first_instance_idx].mesh.tri_count * 3; // check correctness
		model_edge_data_offset += scene_objects[scene_mesh_instances[i].first_instance_idx].mesh.edge_count * 3; // check correctness
	}

	// Common for all objects
	for (uint16_t obj_id = 0; obj_id < SCENE_OBJ_COUNT; obj_id++) {
		scene_objects[obj_id].local_pos = l3d_getZeroVec4();
		scene_objects[obj_id].orientation = l3d_getIdentityQuat();
		// scene_objects[obj_id].wireframe_colour.value = L3D_COLOUR_WHITE;
		scene_objects[obj_id].wireframe_colour = L3D_COLOUR_WHITE;

		// Local orientation unit vectors
		scene_objects[obj_id].u[0] = l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);
		scene_objects[obj_id].u[1] = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
		scene_objects[obj_id].u[2] = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
		scene_objects[obj_id].u[3] = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
		// (parent, children, group, etc) to be added...
	}

	return L3D_OK;
}

static l3d_err_t init_cameras(void) {
	l3d_err_t ret = L3D_OK;
	for (uint16_t i=0; i<SCENE_CAM_COUNT; i++){
		ret = l3d_cam_reset(&scene.cameras[i]);

		if (ret != L3D_OK)
			return ret;
	}

	return ret;
}

l3d_err_t scene_init(void) {
	scene.model_vert_data = scene_model_vertex_data;
	scene.model_tri_data = scene_model_face_data;
	scene.model_edge_data = scene_model_edge_data;
	
	scene.model_vertex_count = SCENE_MODEL_VERT_COUNT;
	scene.model_tri_count = SCENE_MODEL_FACE_COUNT;
	scene.model_edge_count = SCENE_MODEL_EDGE_COUNT;
	
	scene.vertices_world = scene_vertices_world;
	scene.vertices_projected = scene_vertices_projected;
	
	scene.tri_flags = scene_face_flags;
	scene.edge_flags = scene_edge_flags;
	
	scene.transformed_vertex_count = SCENE_TRANSFORMED_VERT_COUNT;
	scene.tri_flag_count = SCENE_FACE_FLAG_COUNT;
	scene.edge_flag_count = SCENE_EDGE_FLAG_COUNT;
	
	scene.objects = scene_objects;
	scene.object_count = SCENE_OBJ_COUNT;
	
	// cube4d_no_faces
	scene_mesh_instances[0].first_instance_idx = 0;
	scene_mesh_instances[0].instance_count = SCENE_OBJ_CUBE4D_NO_FACES_INSTANCE_COUNT;
	
	scene.cameras = scene_cameras;
	scene.camera_count = SCENE_CAM_COUNT;
	
	scene.active_camera_idx = 0;
	
	l3d_err_t ret = init_objects();

	if (ret != L3D_OK)
		return ret;

	ret = init_cameras();

	if (ret != L3D_OK)
		return ret;
	
	l3d_makeProjectionMatrix(&scene.mat_proj, l3d_scene_getActiveCamera(&scene));
	l3d_computeViewMatrix(l3d_scene_getActiveCamera(&scene), &(scene.mat_view));
	return l3d_setupObjects(&scene);
}

