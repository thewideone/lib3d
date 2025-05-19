"""
Before use, type ".\.venv\Scripts\activate"
to activate Python's virtual environment
"""

import sys
import os
import configparser
import numpy as np
from icecream import ic
from math import sqrt
import argparse
import pathlib
from collections import Counter # to check for duplicates

class Edge:
	"""
	Mesh edge class, for more readable computations.
	v1_id, v2_id - ID's of points in space (of Vec4 type)
	face_id (tri_id) - ID of first face the edge was mentioned in (belongs to)
	"""
	def __init__(self, v1_id, v2_id, face_id, is_visible, is_boundary, is_silhouette):
		self.v1_id = v1_id
		self.v2_id = v2_id
		self.face_id = face_id
		self.is_visible = is_visible
		self.is_boundary = is_boundary
		self.is_silhouette = is_silhouette
	
	def __str__(self):
		s = f"{self.v1_id}-{self.v2_id},\tface ID: {self.face_id},\t"

		if self.is_visible:
			s += "visible"
		else:
			s += "invisible"
		s += ",\t"
		if self.is_boundary:
			s += "boundary"
		else:
			s += "not boundary"
		s += ",\t"
		if self.is_silhouette:
			s += "silhouette"
		else:
			s += "not silhouette"

		return s

class Vec4:
	"""
	4D vector class
	x, y, z, h coordinates including h,
	since the library uses homogeneous coordinate system 
	"""
	def __init__(self, x, y, z, h):
		self.x = float(x)
		self.y = float(y)
		self.z = float(z)
		self.h = float(h)

	def __str__(self):
		return f'({self.x}, {self.y}, {self.z}, {self.h})'
	
	def sub(self, v2):
		"""
		Vector subtraction: self - v2
		"""
		return Vec4(self.x - v2.x, self.y - v2.y, self.z - v2.z, 1.0)
	
	def cross_product(self, v2):
		"""
		Vector cross product: self x v2
		"""
		return Vec4( self.y * v2.z - self.z * v2.y,
						self.z * v2.x - self.x * v2.z,
						self.x * v2.y - self.y * v2.x,
						1.0)
	
	def dot_product(self, v2):
		"""
		Vector dot product: self * v2
		"""
		return (self.x*v2.x + self.y*v2.y + self.z*v2.z)
	
	def length(self):
		"""
		Get vector length
		"""
		return sqrt(self.dot_product(self))
	
	def normalise(self):
		"""
		Vector normalisation (makes its length equal to 1.0)
		"""
		length = self.length()
		return Vec4(self.x / length, self.y / length, self.z / length, self.h)

class Mesh:
	"""
	Stores the information about given mesh.
	"""
	def __init__(self, name, instance_count, vertex_array, face_array, edge_array, edge_flags_array):
		self.name = name
		self.instance_count = instance_count

		self.vertex_array = vertex_array
		self.face_array = face_array
		self.edge_array = edge_array
		self.edge_flags_array = edge_flags_array

		self.vertex_count = len(vertex_array)
		self.face_count = len(face_array)
		self.edge_count = len(edge_array)
	
	def __str__(self):
		return f"{self.name}: {self.vertex_count} vertices, {self.face_count} faces, {self.edge_count} edges"

class Scene:
	"""
	Stores scene data
	"""
	def __init__(self, name, meshes, camera_count):
		self.name = name
		self.meshes = meshes

		self.model_vertex_count = 0
		self.model_face_count = 0
		self.model_edge_count = 0
		self.transformed_vertex_count = 0
		self.face_flag_count = 0
		self.edge_flag_count = 0
		self.object_count = 0

		for mesh in self.meshes:
			self.model_vertex_count += mesh.vertex_count
			self.model_face_count += mesh.face_count
			self.model_edge_count += mesh.edge_count
			self.transformed_vertex_count += mesh.vertex_count * mesh.instance_count
			self.face_flag_count += mesh.face_count * mesh.instance_count
			self.edge_flag_count += mesh.edge_count * mesh.instance_count
			self.object_count += mesh.instance_count
		
		self.camera_count = camera_count

def read_lines_from_file(filepath):
	"""
	Read lines from given input file
	"""

	vert_lines = []
	face_lines = []
	# last_vert_line = 0
	# last_face_line = 0

	# vertex_count = 0
	# face_count = 0

	with open(filepath, 'r') as f_in:
		lines = f_in.readlines()

		for line in lines:
			if line[0] == 'v':
				vert_lines.append(line)
			elif line[0] == 'f':
				face_lines.append(line)

	return (vert_lines, face_lines)

def get_vertex_array(config, mesh_name, vert_lines) -> tuple:
	"""
	Get C-style array with vertices data
	"""

	def get_vertex_array_str() -> tuple:
		"""
		Compose a string with the content of C-style array with vertex data
		"""
		vertex_count = len(vert_lines)

		# ic(vert_lines)
		# ic(vertex_count)

		if vertex_count == 0:
			print("Error: no vertices in the input file")
			return ('','')
		
		vert_array_str = ""
		vert_array = []

		last_vert_line = vert_lines[-1]

		for line in vert_lines:
			elements = line.split()

			if config.getboolean('UseFixedPoint'):
				# Convert floating point values into fixed point ones
				fixed_point_type = config['FixedPointType']
				# Get rid of the trailing "_t" in the typename for numpy
				if "_t" in fixed_point_type:
					fixed_point_type = fixed_point_type.replace("_t", "")

				fp_dp = config.getint('FixedPointBinaryDigits')

				values_str = np.array([], dtype=fixed_point_type)
				values = []

				# ic(elements)
				
				for val in elements[1:]:
					# What kind of int is it casted to?
					float_val = float(val) * float(1<<fp_dp) + ( 0.5 if float(val) >= 0 else -0.5 )

					value = 0
					if fixed_point_type == "int8":
						value = np.int8( float_val )
					elif fixed_point_type == "int16":
						value = np.int16( float_val )
					elif fixed_point_type == "int32":
						value = np.int32( float_val )
					elif fixed_point_type == "int64":
						value = np.int64( float_val )
					else:
						print("Error: in get_vertex_data() fixed_point_type is not valid. Aborting")
						return ('','')
					
					values.append(value)
					values_str = np.append( values_str, str(value) )

				vert_array_str += '\t' +  ', '.join(values_str)
				vert_array.append(values)

			else:
				vert_array_str += '\t' + ', '.join( elements[1:] )

			if line != last_vert_line:
				vert_array_str += ',\n'
			else:
				vert_array_str += '\n'
		
		# ic(vert_array_str)

		# ic(vert_array)

		return (vert_array_str, vert_array)
	
	vertex_array_type = config['VertexArrayType']
	vert_array_str, vert_array = get_vertex_array_str()

	s = "const " + vertex_array_type + " mesh_" + mesh_name + "_verts[] = {\n"
	s += vert_array_str
	s += "};\n"

	return (s, vert_array)

def get_face_array(config, mesh_name, face_lines) -> tuple:
	"""
	Get C-style array with faces data
	"""

	def get_face_array_str() -> tuple:
		"""
		Compose a string with the content of C-style array with face data
		"""

		face_count = len(face_lines)

		if face_count == 0:
			print("Error: no faces in the input file")
			return ('','')

		last_face_line = face_lines[-1]

		# ic(face_lines)
		# ic(face_count)

		s = ''
		face_array = []

		for line in face_lines:
			elements_str = line.split()
			elements_int = line.split()

			# Subtract 1 from each vertex ID since indices in C start from 0
			elements_str[1:] = [*map( lambda x: str(int(x)-1), elements_str[1:] )]
			elements_int[1:] = [*map( lambda x: int(x)-1, elements_int[1:] )]

			face_array.append(elements_int[1:])
			
			s += '\t' + ', '.join( elements_str[1:] )

			if line != last_face_line:
				s += ',\n'
			else:
				s += '\n'

		return (s, face_array)

	face_array_type = config['FaceArrayType']
	
	face_array_str, face_array = get_face_array_str()

	s = "const " + face_array_type + " mesh_" + mesh_name + "_faces[] = {\n"
	s += face_array_str
	s += "};\n"

	return (s, face_array)

def get_edge_array(config, mesh_name, vert_array, face_array) -> tuple:
	"""
	Generate a string containing C-style array of edges of the form:
	typedef struct {
		uint16_t verts_ids[2];
		uint16_t tri_id;		// for speed optimization,
								// so far used only by depth test;
								// ID of first encountered tri the edge belongs to
								
		bool is_visible;		// or 2-bit for partial visibility?
		bool is_boundary;		// of a face
		bool is_silhouette;		// of whole mesh projected onto 2D space
	} edge_t;
	"""

	# if len(face_array) % 3:
	# 	print("Error: in get_edge_array(): not using tris as faces. Aborting")
	# 	return ''

	s = ''
	edge_list = []
	edge_flags = []
	edge_flags_str = ''
	face_normals = []

	# ic(face_array)

	# Test vectors:
	# vt1 = Vec4(1.0, 1.0, 0.0, 1.0)
	# vt2 = Vec4(0.0, 0.0, 1.0, 1.0)

	# ic(str(vt1.length()))
	# ic(str(vt1.normalise()))
	# ic(str(vt1.dot_product(vt2)))
	# ic(str(vt1.cross_product(vt2)))
	
	face_id = 0
	for face in face_array:
		# Get face's edges:
		v1_id = face[0]
		v2_id = face[1]
		v3_id = face[2]
		# ic(v1_id, v2_id, v3_id)

		# Compute face's normal:
		v1 = Vec4(vert_array[v1_id][0], vert_array[v1_id][1], vert_array[v1_id][2], 1)
		v2 = Vec4(vert_array[v2_id][0], vert_array[v2_id][1], vert_array[v2_id][2], 1)
		v3 = Vec4(vert_array[v3_id][0], vert_array[v3_id][1], vert_array[v3_id][2], 1)
		# ic(str(v1))
		# ic(str(v2))
		# ic(str(v3))
		
		edge1 = v3.sub(v2)
		edge2 = v1.sub(v2)
		# ic(str(edge1))
		# ic(str(edge2))

		normal = edge1.cross_product(edge2)
		normal = normal.normalise()
		# ic(str(normal))

		face_normals.append(normal)


		# Add edges to the edge_list:
		e1 = Edge(v1_id, v2_id, face_id, is_visible=True, is_boundary=False, is_silhouette=False)
		e2 = Edge(v2_id, v3_id, face_id, is_visible=True, is_boundary=False, is_silhouette=False)
		e3 = Edge(v1_id, v3_id, face_id, is_visible=True, is_boundary=False, is_silhouette=False)

		e1_present = False
		e2_present = False
		e3_present = False

		for edge in edge_list:
			if edge.v1_id == v1_id and edge.v2_id == v2_id or edge.v1_id == v2_id and edge.v2_id == v1_id:
				e1_present = True
			if edge.v1_id == v2_id and edge.v2_id == v3_id or edge.v1_id == v3_id and edge.v2_id == v2_id:
				e2_present = True
			if edge.v1_id == v1_id and edge.v2_id == v3_id or edge.v1_id == v3_id and edge.v2_id == v1_id:
				e3_present = True
		
		if not e1_present:
			edge_list.append(e1)
		if not e2_present:
			edge_list.append(e2)
		if not e3_present:
			edge_list.append(e3)

		# Go to the next face
		face_id += 1
	
	for edge in edge_list:
		# Find one face the edge belongs to
		face1_id = edge.face_id
		# face1 = face_array[face1_id]
		face1_normal = face_normals[face1_id]

		# Find the other face the edge belongs to
		face2_id = -1
		
		entry_id = 0
		for entry in face_array:
			if entry_id != face1_id and\
				((entry[0] == edge.v1_id and entry[1] == edge.v2_id) or (entry[0] == edge.v2_id and entry[1] == edge.v1_id) or\
				(entry[1] == edge.v1_id and entry[2] == edge.v2_id) or (entry[1] == edge.v2_id and entry[2] == edge.v1_id) or\
				(entry[2] == edge.v1_id and entry[0] == edge.v2_id) or (entry[2] == edge.v2_id and entry[0] == edge.v1_id)):
				face2_id = entry_id
				break
			entry_id += 1
		
		if face2_id == -1:
			print(f"Error: in get_edge_array(): could not find the other face the edge ({str(edge)}) belongs to. Returning.")
			return ('','')
		
		# face2 = face_array[face2_id]
		face2_normal = face_normals[face2_id]

		# Compare normals
		# ic(face1_id)
		# ic(face2_id)
		# ic(str(face1_normal))
		# ic(str(face2_normal))
		# ic(face1_normal.dot_product(face2_normal))

		threshold = config.getfloat('BoundaryEdgeThreshold')
		if face1_normal.dot_product(face2_normal) > threshold:
			# ic('not boundary!')
			edge.is_boundary = False
		else:
			# ic('boundary!')
			edge.is_boundary = True


	# ic("edge_list:")
	# ic(len(edge_list))
	# for edge in edge_list:
	# 	ic(str(edge))
	
	# ic("face_normals:")
	# ic(len(face_normals))
	# for normal in face_normals:
	# 	ic(str(normal))

	# Print the edges to the string as C-style array
	edge_array_type = config['EdgeArrayType']
	edge_flags_array_type = config['EdgeFlagsArrayType']
	
	# face_array_str, face_array = get_face_array_str()

	s = "const " + edge_array_type + " mesh_" + mesh_name + "_edges[] = {\n"
	# s += face_array_str

	edge_flags_str = edge_flags_array_type + " mesh_" + mesh_name + "_edge_flags[] = {\n"

	for edge in edge_list:
		# if config.getboolean('PackEdgeFlags'):
		flags : np.int8	# 3 flags fit in int8, no need for larger type
		flags = edge.is_visible << config.getint('EdgeVisibilityFlagBitPos')
		flags |= edge.is_boundary << config.getint('EdgeBoundaryFlagBitPos')
		flags |= edge.is_silhouette << config.getint('EdgeSilhouetteFlagBitPos')
		# flags = edge.is_visible << 2
		# flags |= edge.is_boundary << 1
		# flags |= edge.is_silhouette << 0
		edge_flags.append(flags)
		edge_flags_str += f'\t{flags}'
		s += f'\t{edge.v1_id}, {edge.v2_id}, {edge.face_id}'
			# s += f'\t{edge.v1_id}, {edge.v2_id}, {edge.face_id}, {flags}'
		# else:
			# s += f'\t{edge.v1_id}, {edge.v2_id}, {edge.face_id}, {edge.is_visible}, {edge.is_boundary}, {edge.is_silhouette}'
		
		if edge == edge_list[-1]:
			edge_flags_str += '\n'
			s += '\n'
		else:
			edge_flags_str += ',\n'
			s += ',\n'

	s += "};\n"
	edge_flags_str += "};\n"

	return (s, edge_flags_str, edge_list, edge_flags)

def get_header_comment(config, scene) -> str:
	"""
	Generate header comment
	"""

	s =	 "// \n"
	s += f"// {config['InfoGeneratedBy']}\n"
	s += f"// Scene name: {scene.name}\n"
	if config.getboolean('UseFixedPoint'):
		s += f"// Fixed point type: {config['FixedPointType']}\n"
		s += f"// Fixed point binary digits: {config['FixedPointBinaryDigits']}\n"
	# s += "// Vertex array type: " + config['VertexArrayType'] + "\n"
	# s += "// Face array type: " + config['FaceArrayType'] + "\n"
	s += "// \n"
	return s

def get_includes(scene):
	"""
	Returns includes string for the source file
	"""
	s = f'#include "{scene.name}.h"\n'
	s += '#include "lib3d_config.h"\n'
	s += '#include "lib3d_math.h"\n'
	s += '#include "lib3d_core.h" // for l3d_setupObjects()\n'

	return s

def get_defines(scene):
	"""
	Compose a string containing C-style preprocessor "define" directives
	"""

	s = ''
	s += "// \n"
	s += "// Scene defines\n"
	s += "// \n"

	scene_name = scene.name.upper()

	s += f"#define {scene_name}_MODEL_VERT_COUNT {scene.model_vertex_count}\n"
	s += f"#define {scene_name}_MODEL_FACE_COUNT {scene.model_face_count}\n"
	s += f"#define {scene_name}_MODEL_EDGE_COUNT {scene.model_edge_count}\n"

	s += "\n"
	s += "// WIP: Calculated by scene-model parser (or however I'm gonna call the script),\n"
	s += "// = sum[for each object (model_vertex_count * no_of_object_instances)]\n"

	s += f"#define {scene_name}_TRANSFORMED_VERT_COUNT {scene.transformed_vertex_count}\n"
	s += f"#define {scene_name}_FACE_FLAG_COUNT {scene.face_flag_count}\n"
	s += f"#define {scene_name}_EDGE_FLAG_COUNT {scene.edge_flag_count}\n"

	s += "\n"
	s += "// \n"
	s += "// Object defines\n"
	s += "// \n"
	for mesh in scene.meshes:
		s += "#define MESH_" + mesh.name.upper() + "_VERT_COUNT " + str(mesh.vertex_count) + "\n"
		s += "#define MESH_" + mesh.name.upper() + "_FACE_COUNT " + str(mesh.face_count) + "\n"
		s += "#define MESH_" + mesh.name.upper() + "_EDGE_COUNT " + str(mesh.edge_count) + "\n"

	return s

def get_declarations(config, scene):
	"""
	Returns a string containing declarations of objects and arrays.
	"""
	# TODO: replace string literals e.g. "_vertices_world" with some defined names
	s = ''
	s += f"{config['SceneStructType']} {scene.name};\n"
	s += f"{config['Vec4StructType']} {scene.name}_vertices_world[{scene.name.upper()}_TRANSFORMED_VERT_COUNT];\n"
	s += f"{config['Vec4StructType']} {scene.name}_vertices_projected[{scene.name.upper()}_TRANSFORMED_VERT_COUNT];\n"
	s += f"{config['FaceFlagsArrayType']} {scene.name}_face_flags[{scene.name.upper()}_FACE_FLAG_COUNT];\n"
	s += f"{config['ObjectStructType']} {scene.name}_objects[{scene.name.upper()}_OBJ_COUNT];\n"
	s += f"{config['SceneInstanceDescriptorStructType']} {scene.name}_mesh_instances[{scene.name.upper()}_MESH_COUNT];\n"
	s += f"{config['CameraStructType']} {scene.name}_cameras[{scene.name.upper()}_CAM_COUNT];\n"

	return s
	
def get_init_objects(config, scene) -> str:
	"""
	Returns a string containing init_objects() function
	"""
	s = f"static {config['ErrorType']} init_objects(void) " + "{\n"

	# vertex_data_offset = 0
	# face_data_offset = 0
	# edge_data_offset = 0
	# tr_vertices_offset = 0
	# face_flags_offset = 0
	# edge_flags_offset = 0
	mesh_idx = 0
	for mesh in scene.meshes:
		s += f"\t// {mesh.name}\n"
		s += f"\tfor (uint16_t i = {scene.name}_mesh_instances[{mesh_idx}].first_instance_idx;\n"
		s += f"\t\ti < {scene.name}_mesh_instances[{mesh_idx}].first_instance_idx + {scene.name.upper()}_OBJ_{mesh.name.upper()}_INSTANCE_COUNT;\n"
		s += "\t\ti++)" + " {\n"
		s += f"\t\t{scene.name}_objects[i].mesh.vert_count = MESH_{mesh.name.upper()}_VERT_COUNT;\n"
		s += f"\t\t{scene.name}_objects[i].mesh.tri_count = MESH_{mesh.name.upper()}_FACE_COUNT;\n"
		s += f"\t\t{scene.name}_objects[i].mesh.edge_count = MESH_{mesh.name.upper()}_EDGE_COUNT;\n"
		s += "\t}\n"
		s += "\n"
		mesh_idx += 1

	# s += "uint16_t model_vert_data_offset = 0;\n"
	# s += "uint16_t model_tri_data_offset = 0;\n"
	# s += "uint16_t model_edge_data_offset = 0;\n"
	# s += "uint16_t transformed_vertices_offset = 0;\n"
	# s += "uint16_t tris_flags_offset = 0;\n"
	# s += "uint16_t edges_flags_offset = 0;\n"
	# s += "\n"

	s += """
	uint16_t model_vert_data_offset = 0;
	uint16_t model_tri_data_offset = 0;
	uint16_t model_edge_data_offset = 0;
	uint16_t transformed_vertices_offset = 0;
	uint16_t tris_flags_offset = 0;
	uint16_t edges_flags_offset = 0;\n"""
	s += "\n"

	s += f"\tfor (uint16_t i = 0; i < {scene.name.upper()}_MESH_COUNT; i++)"+" {\n"
	# for mesh in scene.meshes:
	s += f"\t\tfor (uint16_t instance_idx = 0; instance_idx < {scene.name}_mesh_instances[i].instance_count; instance_idx++)"+" {\n"
	s += f"\t\t\tuint16_t obj_id = {scene.name}_mesh_instances[i].first_instance_idx + instance_idx;\n"
	s += f"\t\t\t{scene.name}_objects[obj_id].mesh.model_vert_data_offset = model_vert_data_offset;\n"
	s += f"\t\t\t{scene.name}_objects[obj_id].mesh.model_tri_data_offset = model_tri_data_offset;\n"
	s += f"\t\t\t{scene.name}_objects[obj_id].mesh.model_edge_data_offset = model_edge_data_offset;\n"
	s += "\n"
	s += f"\t\t\t{scene.name}_objects[obj_id].mesh.transformed_vertices_offset = transformed_vertices_offset;\n"
	s += f"\t\t\t{scene.name}_objects[obj_id].mesh.tris_flags_offset = tris_flags_offset;	// not used for now\n"
	s += f"\t\t\t{scene.name}_objects[obj_id].mesh.edges_flags_offset = edges_flags_offset;\n"
	s += "\n"
	s += f"\t\t\ttransformed_vertices_offset += {scene.name}_objects[obj_id].mesh.vert_count;\n"
	s += f"\t\t\ttris_flags_offset += {scene.name}_objects[obj_id].mesh.tri_count;\n"
	s += f"\t\t\tedges_flags_offset += {scene.name}_objects[obj_id].mesh.edge_count;\n"
	s += "\t\t}\n"
	s += "\n"
	s += "\t\t// Update offsets\n"
	s += f"\t\tmodel_vert_data_offset += {scene.name}_objects[{scene.name}_mesh_instances[i].first_instance_idx].mesh.vert_count * 3; // check correctness\n"
	s += f"\t\tmodel_tri_data_offset += {scene.name}_objects[{scene.name}_mesh_instances[i].first_instance_idx].mesh.tri_count * 3; // check correctness\n"
	s += f"\t\tmodel_edge_data_offset += {scene.name}_objects[{scene.name}_mesh_instances[i].first_instance_idx].mesh.edge_count * 3; // check correctness\n"
	s += "\t}\n"

	s += "\n"
	s += "\t// Common for all objects\n"
	s += f"\tfor (uint16_t obj_id = 0; obj_id < {scene.name.upper()}_OBJ_COUNT; obj_id++)"+" {\n"
	s += f"\t\t{scene.name}_objects[obj_id].local_pos = l3d_getZeroVec4();\n"
	s += f"\t\t{scene.name}_objects[obj_id].orientation = l3d_getIdentityQuat();\n"
	s += f"\t\t// {scene.name}_objects[obj_id].wireframe_colour.value = L3D_COLOUR_WHITE;\n"
	s += f"\t\t{scene.name}_objects[obj_id].wireframe_colour = L3D_COLOUR_WHITE;\n"
	# s += f"\t\t// {scene.name}_objects[obj_id].fill_colour.value = L3D_COLOUR_WHITE;\n"
	# s += f"\t\t{scene.name}_objects[obj_id].fill_colour = L3D_COLOUR_WHITE;	// to be removed\n"
	s += "\n"
	s += "\t\t// Local orientation unit vectors\n"
	s += f"\t\t{scene.name}_objects[obj_id].u[0] = l3d_getVec4FromFloat(0.0f, 0.0f, 0.0f, 1.0f);\n"
	s += f"\t\t{scene.name}_objects[obj_id].u[1] = l3d_getVec4FromFloat(1.0f, 0.0f, 0.0f, 1.0f);\n"
	s += f"\t\t{scene.name}_objects[obj_id].u[2] = l3d_getVec4FromFloat(0.0f, 1.0f, 0.0f, 1.0f);\n"
	s += f"\t\t{scene.name}_objects[obj_id].u[3] = l3d_getVec4FromFloat(0.0f, 0.0f, 1.0f, 1.0f);\n"
	s += "\t\t// (parent, children, group, etc) to be added...\n"
	s += "\t}\n"

	# s += f"\t\t{scene.name}_objects[{i}].mesh.model_vert_data_offset = {vertex_data_offset};\n"
	# s += f"\t\t{scene.name}_objects[{i}].mesh.model_tri_data_offset = {face_data_offset};\n"
	# s += f"\t\t{scene.name}_objects[{i}].mesh.model_edge_data_offset = {edge_data_offset};\n"
	
		# vertex_data_offset += mesh.vertex_count
		# face_data_offset += mesh.face_count
		# edge_data_offset += mesh.edge_count
		# tr_vertices_offset += mesh.vertex_count * mesh.instance_count
		# face_flags_offset += mesh.face_count * mesh.instance_count
		# edge_flags_offset += mesh.edge_count * mesh.instance_count

	s += "\n"
	s += "\treturn L3D_OK;\n"
	s += "}\n"
	return s

def get_init_cameras(config, scene) -> str:
	"""
	Returns string containint init_cameras() function
	"""
	s = f"static {config['ErrorType']} init_cameras(void) " + "{\n"
	s += "\tl3d_err_t ret = L3D_OK;\n"
	s += f"\tfor (uint16_t i=0; i<{scene.name.upper()}_CAM_COUNT; i++)" + "{\n"
	s += f"\t\tret = l3d_cam_reset(&{scene.name}.cameras[i]);\n"
	s += """
		if (ret != L3D_OK)
			return ret;
	}

	return ret;\n"""
	s += "}\n"

	return s

def get_scene_init(config, scene) -> str:
	"""
	Returns a string containing scene initialisation function.
	"""
	s = f"{config['ErrorType']} {scene.name}_init(void) " + "{\n"
	s += f"\t{scene.name}.model_vert_data = {scene.name}_model_vertex_data;\n"
	s += f"\t{scene.name}.model_tri_data = {scene.name}_model_face_data;\n"
	s += f"\t{scene.name}.model_edge_data = {scene.name}_model_edge_data;\n"
	s += f"\t\n"
	s += f"\t{scene.name}.model_vertex_count = {scene.name.upper()}_MODEL_VERT_COUNT;\n"
	s += f"\t{scene.name}.model_tri_count = {scene.name.upper()}_MODEL_FACE_COUNT;\n"
	s += f"\t{scene.name}.model_edge_count = {scene.name.upper()}_MODEL_EDGE_COUNT;\n"
	s += f"\t\n"
	s += f"\t{scene.name}.vertices_world = {scene.name}_vertices_world;\n"
	s += f"\t{scene.name}.vertices_projected = {scene.name}_vertices_projected;\n"
	s += f"\t\n"
	s += f"\t{scene.name}.tri_flags = {scene.name}_face_flags;\n"
	s += f"\t{scene.name}.edge_flags = {scene.name}_edge_flags;\n"
	s += f"\t\n"
	s += f"\t{scene.name}.transformed_vertex_count = {scene.name.upper()}_TRANSFORMED_VERT_COUNT;\n"
	s += f"\t{scene.name}.tri_flag_count = {scene.name.upper()}_FACE_FLAG_COUNT;\n"
	s += f"\t{scene.name}.edge_flag_count = {scene.name.upper()}_EDGE_FLAG_COUNT;\n"
	s += f"\t\n"
	s += f"\t{scene.name}.objects = {scene.name}_objects;\n"
	s += f"\t{scene.name}.object_count = {scene.name.upper()}_OBJ_COUNT;\n"
	s += f"\t\n"

	first_mesh_instance_idx = 0
	mesh_idx = 0
	for mesh in scene.meshes:
		s += f"\t// {mesh.name}\n"
		s += f"\t{scene.name}_mesh_instances[{mesh_idx}].first_instance_idx = {first_mesh_instance_idx};\n"
		s += f"\t{scene.name}_mesh_instances[{mesh_idx}].instance_count = {scene.name.upper()}_OBJ_{mesh.name.upper()}_INSTANCE_COUNT;\n"
		first_mesh_instance_idx += mesh.instance_count
		mesh_idx += 1

	s += f"\t\n"
	s += f"\t{scene.name}.cameras = {scene.name}_cameras;\n"
	s += f"\t{scene.name}.camera_count = {scene.name.upper()}_CAM_COUNT;\n"
	s += f"\t\n"
	s += f"\t{scene.name}.active_camera = &{scene.name}.cameras[0];\n"
	s += f"\t\n"

	s += f"\t{config['ErrorType']} ret = init_objects();\n"
	# s += f"\t\n"
	s += """
	if (ret != L3D_OK)
		return ret;

	ret = init_cameras();

	if (ret != L3D_OK)
		return ret;
	\n"""
	s += f"\tl3d_makeProjectionMatrix(&{scene.name}.mat_proj, {scene.name}.active_camera);\n"
	s += f"\tl3d_computeViewMatrix({scene.name}.active_camera, &({scene.name}.mat_view));\n"
	s += f"\treturn l3d_setupObjects(&{scene.name});\n"
	s += "}\n"

	return s

def get_header_file_content(config, scene) -> str:
	"""
	Returns string with the content of the output header file
	"""
	
	s = ''

	s += f"#ifndef _{scene.name.upper()}_H_\n"
	s += f"#define _{scene.name.upper()}_H_\n"
	s += '\n'
	s += get_header_comment(config, scene)
	s += '\n'
	s += '#include "lib3d_scene.h"\n'

	s += "\n"
	for mesh in scene.meshes:
		s += f"#define {scene.name.upper()}_OBJ_{mesh.name.upper()}_INSTANCE_COUNT {mesh.instance_count}\n"

	s += "\n"
	# s += "// \n"
	s += "// Object instances ID's\n"
	# s += "// \n"
	obj_id = 0
	for mesh in scene.meshes:
		for instance_id in range(mesh.instance_count):
			s += f"#define {scene.name.upper()}_OBJ_{mesh.name.upper()}_I{instance_id}_ID {obj_id}\n"
			obj_id += 1
	
	s += "\n"
	s += "// Number of different meshes in the scene\n"
	s += f"#define {scene.name.upper()}_MESH_COUNT {len(scene.meshes)}\n"
	s += "// Total number of objects in the scene (different meshes * their no. of instances)\n"
	s += f"#define {scene.name.upper()}_OBJ_COUNT {scene.object_count}\n"
	s += "// Total number of cameras in the scene\n"
	s += f"#define {scene.name.upper()}_CAM_COUNT {scene.camera_count}\n"

	s += '\n'
	s += f"extern {config['SceneStructType']} {scene.name};\n"
	s += '\n'
	s += f"{config['ErrorType']} {scene.name}_init(void);\n"
	s += '\n'
	s += f"#endif // _{scene.name.upper()}_H_\n"

	return s

def get_source_arrays(config, scene) -> str:
	"""
	Returns string with arrays computed from meshes
	"""

	s = ''
	vertex_array_type = config['VertexArrayType']
	s += f"const {vertex_array_type} {scene.name}_model_vertex_data[]" + " = {\n"

	for mesh in scene.meshes:
		s += f"\t// {mesh.name}\n"
		for vertex in mesh.vertex_array:
			s += '\t'
			for i in range(3):	# pos in each axis in 3D
				s += str(vertex[i])
				s += ', '
			s += '\n'
	s += "};\n"
	s += '\n'

	face_array_type = config['FaceArrayType']
	s += f"const {face_array_type} {scene.name}_model_face_data[]" + " = {\n"

	faces_offset = 0
	for mesh in scene.meshes:
		s += f"\t// {mesh.name}\n"
		for face in mesh.face_array:
			s += '\t'
			for i in range(3):	# triangle has 3 vertices
				vertex_id = face[i] #+ faces_offset
				s += str(vertex_id)
				s += ', '
			s += '\n'
		faces_offset += mesh.vertex_count
	s += "};\n"
	s += '\n'

	edge_array_type = config['EdgeArrayType']
	s += f"const {edge_array_type} {scene.name}_model_edge_data[]" + " = {\n"

	vertex_offset = 0
	faces_offset = 0
	for mesh in scene.meshes:
		s += f"\t// {mesh.name}\n"
		for edge in mesh.edge_array:
			s += '\t'
			vertex1_id = edge.v1_id #+ vertex_offset
			vertex2_id = edge.v2_id #+ vertex_offset
			face_id = edge.face_id #+ faces_offset
			s += f"{vertex1_id}, {vertex2_id}, {face_id},"
			s += '\n'
		vertex_offset += mesh.vertex_count
		faces_offset += mesh.face_count
	s += "};\n"
	s += '\n'

	edge_flags_array_type = config['EdgeFlagsArrayType']
	s += f"{edge_flags_array_type} {scene.name}_edge_flags[]" + " = {\n"

	for mesh in scene.meshes:
		for instance_no in range(mesh.instance_count):
			s += f"\t// {mesh.name} instance {instance_no}\n"
			for flag in mesh.edge_flags_array:
				s += '\t'
				s += f"{flag},"
				s += '\n'
	s += "};\n"
	s += '\n'

	return s

def get_source_file_content(config, scene) -> str:
	"""
	Returns string with the content of the output source file
	"""

	s = ""
	s += get_includes(scene)
	s += '\n'
	s += get_defines(scene)
	s += "\n"
	s += get_source_arrays(config, scene)
	s += "\n"
	# print(defines)
	s += get_declarations(config, scene)
	s += "\n"
	# print(declarations)
	s += get_init_objects(config, scene)
	s += "\n"
	# print(init_objects_str)
	s += get_init_cameras(config, scene)
	s += "\n"
	# print(init_cameras_str)
	s += get_scene_init(config, scene)
	s += "\n"

	return s

def file_path(path):
	"""
	Validates argument path.
	Used for argparse module.
	"""
	if os.path.exists(path):
		return path
	else:
		raise argparse.ArgumentTypeError(f"readable_file:{path} is not a valid path")

# def dir_path(path):
# 	if os.path.isdir(path):
# 		return path
# 	else:
# 		raise argparse.ArgumentTypeError(f"readable_dir:{path} is not a valid path")	

def main() -> None:
	"""
	Read input file, perform calculations and save results in output files.
	"""

	# TODO: change at least "bottom text" XD
	parser = argparse.ArgumentParser(
				prog="model_parser.py",
				description="Input: obj files with 3D models, output: scene description files for LIB3D",
				# epilog="Bottom text (epilog)",
				usage="%(prog)s [options]"
			)
	
	# Taken from: https://stackoverflow.com/a/60796254
	# parser.add_argument('--models', action='store', type=argparse.FileType('r', encoding='utf-8'), nargs='+', help="specify input .obj files")
	parser.add_argument('models', action='store', type=file_path, nargs='+', help="input .obj files")
	parser.add_argument('output_dir', type=pathlib.Path, nargs=1, help="output directory where new files will be created")
	parser.add_argument('-i', '--instances', action='store', type=int, nargs='+', help="the number of instances of each model")
	parser.add_argument('-o', '--output', help="scene name")
	parser.add_argument('-c', '--cameras', type=int, help="number of cameras in the scene")

	args = parser.parse_args()

	models = args.models
	output_dir = args.output_dir[0]
	
	# Check for duplicates
	# Taken from https://stackoverflow.com/a/11236042
	duplicate_list = [k for k,v in Counter(args.models).items() if v>1]
	if len(duplicate_list) > 0:
		parser.error('duplicate models specified')

	if args.instances is None:
		# Numbers of instances of each mesh not specified
		# So take 1 as default
		instances_counts = [1 for _ in models]
	elif len(args.instances) > len(models):
		# More numbers of instances than models
		parser.error('specified more numbers of instances than number of models')
	else:
		instances_counts = args.instances
		# Add padding with default instance count
		# if some are not specified
		instances_counts += [1] * (len(models) - len(instances_counts))
	
	if args.output is None:
		# Scene name not specified
		scene_name = "scene"
	else:
		scene_name = args.output
	
	if args.cameras is None:
		# Number of cameras not specified
		no_of_cameras = 1
	else:
		no_of_cameras = args.cameras

	config = configparser.ConfigParser()
	# Read config
	config.read('config.ini')

	use_fixed_point = config['DEFAULT'].getboolean('UseFixedPoint')
	
	if use_fixed_point:
		current_config_section = config['UseFixedPoint']
	else:
		current_config_section = config['UseFloatingPoint']

	meshes = []
	mesh_idx = 0
	for path in models:
		mesh_name = os.path.basename(path).split('.')[0]
		vert_lines, face_lines = read_lines_from_file(path)

		vert_array_str, vert_array = get_vertex_array(current_config_section, mesh_name, vert_lines)
		face_array_str, face_array = get_face_array(current_config_section, mesh_name, face_lines)
		edge_array_str, edge_flags_str, *raw_arrays = get_edge_array(current_config_section, mesh_name, vert_array, face_array)

		mesh = Mesh(mesh_name, instances_counts[mesh_idx], vert_array, face_array, edge_array=raw_arrays[0], edge_flags_array=raw_arrays[1])
		meshes.append(mesh)
		mesh_idx += 1

	scene = Scene(name=scene_name, meshes=meshes, camera_count=no_of_cameras)
	
	source_file_content = get_source_file_content(current_config_section, scene)
	header_file_content = get_header_file_content(current_config_section, scene)

	# Create filenames
	header_filename = scene_name + '.h'
	source_filename = scene_name + '.c'

	# Write the header file
	output_file = output_dir / pathlib.Path(header_filename) # concat with / operator
	with output_file.open("w", encoding='utf-8') as f:
		f.write(header_file_content)

	# Write the source file
	output_file = output_dir / pathlib.Path(source_filename) # concat with / operator
	with output_file.open("w", encoding='utf-8') as f:
		f.write(source_file_content)

if __name__ == "__main__":
	main()
