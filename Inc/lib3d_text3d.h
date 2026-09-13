#ifndef _L3D_FONT3D_H_
#define _L3D_FONT3D_H_

#include "lib3d_config.h"
#include "lib3d_math.h"
#include "lib3d_mesh.h"
#include "lib3d_obj3d.h"

typedef struct
{
	const uint8_t char_count;
	const uint16_t *model_vert_counts;
	const uint16_t *model_face_counts;
	const uint16_t *model_edge_counts;
	const int8_t *model_vertex_data;
	const uint16_t *model_face_data;
	const uint16_t *model_edge_data;
	const uint8_t *edges_flags;
} l3d_font_descriptor_t;

// 
// Dynamic 3D character
// Can change displayed character at runtime.
// 
typedef struct
{
	// uint8_t tris_flags[L3D_FONT3D_MAX_FACE_COUNT];	// not used anyway yet
	uint8_t edges_flags[L3D_FONT3D_MAX_EDGE_COUNT];
	l3d_vec4_t vertices_world[L3D_FONT3D_MAX_VERT_COUNT];
	l3d_vec4_t vertices_projected[L3D_FONT3D_MAX_VERT_COUNT];



	// uint16_t model_vert_data_offset;
	// uint16_t model_tri_data_offset;
	// uint16_t model_edge_data_offset;

	// uint16_t transformed_vertices_offset; // not needed, as the transformed vertices are stored inside this struct
	// uint16_t tris_flags_offset;
	// uint16_t edges_flags_offset;

	// uint16_t tri_count;
	// uint16_t vert_count;
	// uint16_t edge_count;

	// Contains all of the above
	// l3d_mesh_t mesh;

	// Contains all of the above + extra needed stuff
	l3d_obj3d_t obj3d;

	// Pointer to font descriptor,
	// which contains all of the font data arrays
	l3d_font_descriptor_t *font_desc;	

	// Pointers to font arrays
	// uint16_t *font_model_vert_counts;
	// uint16_t *font_model_face_counts;
	// uint16_t *font_model_edge_counts;
	// int8_t *font_model_vertex_data;
	// uint16_t *font_model_face_data;
	// uint16_t *font_model_edge_data;

	l3d_rtnl_t scale;	// scale factor for the character size

} l3d_char3d_t;

typedef struct
{
	l3d_char3d_t *chars;	// [L3D_FONT3D_MAX_CHAR_COUNT];
	uint8_t char_count;

	// From l3d_obj3d_t:

	bool updated;

	l3d_colour_t wireframe_colour;
	// l3d_colour_t fill_colour;

	// Object properties:
	l3d_vec4_t local_pos;	// actually global,
							// since object space has not been implemented yet
	l3d_quat_t orientation;

	// Object orientation markers (local coordinate system vectors)
	// Can be drawn
	// Probably will be used for object gizmos
	// These four stand for origin, x, y, and z respectively
	l3d_vec4_t u[4];	// u[0] seems to be redundant because it's equal to local_pos
	l3d_vec4_t u_world[4];
	l3d_vec4_t u_proj[4];
} l3d_str3d_t;

l3d_err_t l3d_char3d_init(l3d_char3d_t *char3d, const l3d_font_descriptor_t *font_desc);
// l3d_char3d_t *l3d_char3d_create(const char *c);	// would use malloc or sth
l3d_err_t l3d_char3d_getChar(l3d_char3d_t *char3d, char *c);
l3d_err_t l3d_char3d_setChar(l3d_char3d_t *char3d, char c);
l3d_err_t l3d_char3d_getLocalPos(l3d_char3d_t *char3d, l3d_vec4_t *pos);
l3d_err_t l3d_char3d_setLocalPos(l3d_char3d_t *char3d, const l3d_vec4_t *pos);
// ...

// l3d_str3d_t *l3d_str3d_create(const char *str, uint8_t len);	// would use malloc or sth
l3d_err_t l3d_str3d_getChar(l3d_str3d_t *str3d, uint8_t char_index, char *c);
l3d_err_t l3d_str3d_setChar(l3d_str3d_t *str3d, uint8_t char_index, char c);
l3d_err_t l3d_str3d_getStr(l3d_str3d_t *str3d, char *str, uint8_t length);
l3d_err_t l3d_str3d_setStr(l3d_str3d_t *str3d, const char *str);
// ...

// Zephyr-style static initialization macro for l3d_char3d_t
#define L3D_DEFINE_CHAR3D(name, c)	\
	l3d_char3d_t name; 				\
	l3d_char3d_setChar(&name, c);

// Zephyr-style static initialization macro for l3d_str3d_t
#define L3D_DEFINE_STR3D(name, str, length)	\
	l3d_char3d_t name##_chars[length];		\
	l3d_str3d_t name = {					\
		.chars = name##_chars,				\
		.char_count = length,				\
	}										\
	for (uint8_t i = 0; i < name.char_count && i < sizeof(str) - 1; i++) {	\
		l3d_err_t ret = l3d_char3d_setChar(&name##_chars[i], str[i]);		\
		if (ret != L3D_OK) {				\
			return ret;	/* can't be inside macro, as it can be called in global scope */	\
		}									\
	}

#endif // _L3D_FONT3D_H_