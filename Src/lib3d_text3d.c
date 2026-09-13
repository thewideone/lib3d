/* 
 * lib3d_text3d.c
 *
 *  Created on: 2024-06-05
 * 
 * Usage in order:
 * 	l3d_char3d_init()
 * 	l3d_char3d_setChar()
 * 	l3d_transformObjectIntoWorldSpace()
 * 	transformVertexArrayIntoViewSpace()
 * 
 * If font is to be changed, the above steps must be repeated.
 * 
*/

#include "lib3d_text3d.h"
// #include "../model_parser/test_output/font_3d_simple/exemplary_font_3d_simple.h"

#include "lib3d_util.h"

// 
// Does not set to any character by default.
// 
l3d_err_t l3d_char3d_init(l3d_char3d_t *char3d, const l3d_font_descriptor_t *font_desc) {
	// Common for all objects
	// (from generated <scene_descriptor>.c)
	char3d->obj3d.local_pos = l3d_getZeroVec4();
	char3d->obj3d.orientation = l3d_getIdentityQuat();
	// char3d->obj3d.wireframe_colour.value = L3D_COLOUR_WHITE;
	char3d->obj3d.wireframe_colour = L3D_COLOUR_WHITE;

	// Local orientation unit vectors
	char3d->obj3d.u[0] = l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);
	char3d->obj3d.u[1] = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);
	char3d->obj3d.u[2] = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);
	char3d->obj3d.u[3] = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);
	// (parent, children, group, etc) to be added...

	char3d->font_desc = (l3d_font_descriptor_t *)font_desc;
	char3d->scale = l3d_floatToRational(1.0f);

	return L3D_OK;
}

l3d_err_t l3d_char3d_setChar(l3d_char3d_t *char3d, char c) {
	if (char3d == NULL || char3d->font_desc == NULL)
		return L3D_DATA_EMPTY;
	
	// Compute index of the character in the font arrays
	uint8_t char_index;
	// if (c >= 'A' && c <= 'Z') {
	// 	char_index = (uint8_t)c - 'A';
	// }
	// else if (c >= 'a' && c <= 'z') {
	// 	char_index = (uint8_t)c - 'a';
	// }
	// else if (c >= '0' && c <= '9') {
	// 	char_index = (uint8_t)c - '0';
	// }
	if (c >= ' ' && c <= '`') {	// ASCII printable characters from space (32) to grave accent (96)
		char_index = (uint8_t)c - ' ';
	}
	// Convert lowercase letters to uppercase
	// as the former are not implemented at the moment
	else if (c >= 'a' && c <= '~') {
		// char_index = (uint8_t)c - ('a' - ' ') - 'A';	// 97('a') - 32(' ') - 65('A') = 0
		char_index = (uint8_t)c - ('a' - 'A') - ' ';
	}
	// else if (c >= '{' && c <= '~') {	// ASCII printable characters from left brace (123) to tilde (126)
	// 	char_index = (uint8_t)c - ' ';
	// }
	else {
		return L3D_WRONG_PARAM;
	}

	L3D_DEBUG_PRINT("char '%c' (ASCII %d) -> char_index %d\n", c, (int)c, (int)char_index);

	// Buffer overflow catch
	if (char_index >= char3d->font_desc->char_count) {
		return L3D_WRONG_PARAM;
	}

	// 
	// TODO:
	// Make these arrays passed as arguments for example
	// 
	// char3d->font_model_vert_counts = (uint16_t *)font_3d_simple_model_vert_counts;
	// char3d->font_model_face_counts = (uint16_t *)font_3d_simple_model_face_counts;
	// char3d->font_model_edge_counts = (uint16_t *)font_3d_simple_model_edge_counts;
	// char3d->font_model_vertex_data = (int8_t *)char3d->font_desc->model_vertex_data;
	// char3d->font_model_face_data = (uint16_t *)char3d->font_desc->model_face_data;
	// char3d->font_model_edge_data = (uint16_t *)char3d->font_desc->model_edge_data;

	char3d->obj3d.mesh.vert_count = char3d->font_desc->model_vert_counts[char_index];
	char3d->obj3d.mesh.tri_count  = char3d->font_desc->model_face_counts[char_index];
	char3d->obj3d.mesh.edge_count = char3d->font_desc->model_edge_counts[char_index];

	L3D_DEBUG_PRINT("vert_count %d, tri_count %d, edge_count %d\n", 
		(int)char3d->obj3d.mesh.vert_count, (int)char3d->obj3d.mesh.tri_count, (int)char3d->obj3d.mesh.edge_count);

	uint16_t model_vert_data_offset = 0;
	uint16_t model_tri_data_offset = 0;
	uint16_t model_edge_data_offset = 0;
	
	uint16_t edges_flags_offset = 0;

	// Compute model data offsets
	for (uint8_t model_idx = 0; model_idx < char_index; model_idx++) {
		model_vert_data_offset += char3d->font_desc->model_vert_counts[model_idx] * 3;
		model_tri_data_offset  += char3d->font_desc->model_face_counts[model_idx] * 3;
		model_edge_data_offset += char3d->font_desc->model_edge_counts[model_idx] * 3;
		edges_flags_offset     += char3d->font_desc->model_edge_counts[model_idx];
	}

	char3d->obj3d.mesh.model_vert_data_offset = model_vert_data_offset;
	char3d->obj3d.mesh.model_tri_data_offset = model_tri_data_offset;
	char3d->obj3d.mesh.model_edge_data_offset = model_edge_data_offset;
	char3d->obj3d.mesh.edges_flags_offset = edges_flags_offset;

	L3D_DEBUG_PRINT("model_vert_data_offset %d, model_tri_data_offset %d, model_edge_data_offset %d, edges_flags_offset %d\n", 
		(int)char3d->obj3d.mesh.model_vert_data_offset, (int)char3d->obj3d.mesh.model_tri_data_offset, 
		(int)char3d->obj3d.mesh.model_edge_data_offset, (int)char3d->obj3d.mesh.edges_flags_offset);

	// Copy edge flags
	for (uint16_t i = 0; i < char3d->obj3d.mesh.edge_count; i++) {
		char3d->edges_flags[i] = char3d->font_desc->edges_flags[edges_flags_offset + i];
		// L3D_DEBUG_PRINT("edges_flags[%d] = %d\n", (int)i, (int)char3d->edges_flags[i]);
	}

	char3d->obj3d.updated = true;	// mark as updated, so that the next transform will be performed

	// Zero-out not used parameters
	char3d->obj3d.mesh.edges_flags_offset = 0;
	char3d->obj3d.mesh.tris_flags_offset = 0;
	char3d->obj3d.mesh.transformed_vertices_offset = 0;

	return L3D_OK;
}