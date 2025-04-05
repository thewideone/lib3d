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

# Save config
# with open('config.ini', 'w') as configfile:
# 	config.write(configfile)

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
	def __init__(self, name, vertex_array, face_array, edge_array, edge_flags_array):
		self.name = name

		self.vertex_array = vertex_array
		self.face_array = face_array
		self.edge_array = edge_array
		self.edge_flags_array = edge_flags_array

		self.vertex_count = len(vertex_array)
		self.face_count = len(face_array)
		self.edge_count = len(edge_array)
	
	def __str__(self):
		return f"{self.name}: {self.vertex_count} vertices, {self.face_count} faces, {self.edge_count} edges"

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
	

def get_header_comment(config, mesh_name) -> str:
	"""
	Generate header comment
	"""

	s =	 "// \n"
	s += "// " + config['InfoGeneratedBy'] + "\n"
	s += "// Mesh name: '" + mesh_name + "'\n"
	if config.getboolean('UseFixedPoint'):
		s += "// Fixed point type: " + config['FixedPointType'] + "\n"
		s += "// Fixed point binary digits: " + config['FixedPointBinaryDigits'] + "\n"
	# s += "// Vertex array type: " + config['VertexArrayType'] + "\n"
	# s += "// Face array type: " + config['FaceArrayType'] + "\n"
	s += "// \n"
	return s

def get_defines(mesh_name, vertex_count, face_count):
	"""
	Compose a string containing C-style preprocessor "define" directives
	"""
	s = "#define MESH_" + mesh_name.upper() + "_VERT_COUNT " + str(vertex_count) + "\n"
	s += "#define MESH_" + mesh_name.upper() + "_FACE_COUNT " + str(face_count) + "\n"

	return s

def get_header_file_content(config, mesh_name, vertex_count, face_count) -> str:
	"""
	Generate content of the output header file
	"""

	use_fixed_point = config['DEFAULT'].getboolean('UseFixedPoint')
	
	if use_fixed_point:
		current_config_section = config['UseFixedPoint']
	else:
		current_config_section = config['UseFloatingPoint']
	
	header_def_symbol = "_MESH_" + mesh_name.upper() + "_H_"
	s = ''

	s += "#ifndef " + header_def_symbol + '\n'
	s += "#define " + header_def_symbol + '\n'
	s += '\n'
	s += get_header_comment(current_config_section, mesh_name)
	s += '\n'
	s += '#include "lib3d_config.h"\n'
	s += '\n'
	s += get_defines(mesh_name, vertex_count, face_count)
	s += '\n'
	s += "extern const " + config['DEFAULT']['RationalType']  + " mesh_" + mesh_name + "_verts[];\n"
	s += "extern const " + config['DEFAULT']['FaceArrayType'] + " mesh_" + mesh_name + "_faces[];\n"
	s += "extern const " + config['DEFAULT']['EdgeArrayType'] + " mesh_" + mesh_name + "_edges[];\n"
	s += '\n'
	s += "#endif // " + header_def_symbol + '\n'

	
	# vertex_array_fixed = get_vertex_array(fixed_point_config, vert_lines, mesh_name)
	# vertex_array_floating = get_vertex_array(floating_point_config, vert_lines, mesh_name)
	# face_array = get_face_array(fixed_point_config, face_lines, mesh_name)

	# ic(defines)
	# ic(vertex_array_fixed)
	# ic(vertex_array_floating)
	# ic(face_array)

	# ic(s)

	return s

# def get_source_file_content(config, header_filename, mesh_name, vert_lines, face_lines) -> str:
# 	"""
# 	Generate content of the output source file
# 	"""

# 	use_fixed_point = config['DEFAULT'].getboolean('UseFixedPoint')
	
# 	if use_fixed_point:
# 		current_config_section = config['UseFixedPoint']
# 	else:
# 		current_config_section = config['UseFloatingPoint']

# 	face_array_str, face_array = get_face_array(current_config_section, mesh_name, face_lines)
# 	vert_array_str, vert_array = get_vertex_array(current_config_section, mesh_name, vert_lines)
# 	edge_array_str, edge_flags_str, *raw_arrays = get_edge_array(current_config_section, mesh_name, vert_array, face_array)

# 	# ic(face_array_str)
# 	# ic(face_array)

# 	s = ''
# 	s += '#include "' + header_filename + '"\n'
# 	s += '\n'
# 	s += vert_array_str
# 	s += '\n'
# 	s += face_array_str
# 	s += '\n'
# 	s += edge_array_str
# 	s += '\n'
# 	s += edge_flags_str
# 	s += '\n'

# 	# ic(edge_array_str)


# 	return s

def get_source_file_content(config, header_filename, meshes) -> str:
	"""
	Generate content of the output source file
	"""

	use_fixed_point = config['DEFAULT'].getboolean('UseFixedPoint')
	
	if use_fixed_point:
		current_config_section = config['UseFixedPoint']
	else:
		current_config_section = config['UseFloatingPoint']

	s = ''
	s += '#include "' + header_filename + '"\n'
	s += '\n'
	vertex_array_type = current_config_section['VertexArrayType']
	s += f"const {vertex_array_type} {header_filename}_verts[]" + " = {\n"

	for mesh in meshes:
		s += f"\t// {mesh.name}\n"
		for vertex in mesh.vertex_array:
			s += '\t'
			for i in range(3):	# pos in each axis in 3D
				s += str(vertex[i])
				s += ', '
			s += '\n'
	s += "};\n"
	s += '\n'

	face_array_type = current_config_section['FaceArrayType']
	s += f"const {face_array_type} {header_filename}_faces[]" + " = {\n"

	faces_offset = 0
	for mesh in meshes:
		s += f"\t// {mesh.name}\n"
		for face in mesh.face_array:
			s += '\t'
			for i in range(3):	# triangle has 3 vertices
				vertex_id = face[i] + faces_offset
				s += str(vertex_id)
				s += ', '
			s += '\n'
		faces_offset += mesh.vertex_count
	s += "};\n"
	s += '\n'

	edge_array_type = current_config_section['EdgeArrayType']
	s += f"const {edge_array_type} {header_filename}_edges[]" + " = {\n"

	vertex_offset = 0
	faces_offset = 0
	for mesh in meshes:
		s += f"\t// {mesh.name}\n"
		for edge in mesh.edge_array:
			s += '\t'
			vertex1_id = edge.v1_id + vertex_offset
			vertex2_id = edge.v2_id + vertex_offset
			face_id = edge.face_id + faces_offset
			s += f"{vertex1_id}, {vertex2_id}, {face_id},"
			s += '\n'
		vertex_offset += mesh.vertex_count
		faces_offset += mesh.face_count
	s += "};\n"
	s += '\n'

	edge_flags_array_type = current_config_section['EdgeFlagsArrayType']
	s += f"const {edge_flags_array_type} {header_filename}_edge_flags[]" + " = {\n"

	for mesh in meshes:
		s += f"\t// {mesh.name}\n"
		for flag in mesh.edge_flags_array:
			s += '\t'
			s += f"{flag},"
			s += '\n'
	s += "};\n"
	s += '\n'

	return s

def file_path(path):
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

	parser = argparse.ArgumentParser(
				prog="model_parser.py",
				description="Input: obj files with 3D models, output: scene description files for LIB3D",
				epilog="Bottom text (epilog)",
				usage="%(prog)s [options]"
			)
	
	# Taken from: https://stackoverflow.com/a/60796254
	# parser.add_argument('--models', action='store', type=argparse.FileType('r', encoding='utf-8'), nargs='+', help="specify input .obj files")
	parser.add_argument('--models', action='store', type=file_path, nargs='+', help="specify input .obj files")
	parser.add_argument('--output-dir', type=pathlib.Path, nargs=1, help="specify output directory where new files will be created")
	parser.add_argument('-o', help="output files name")

	args = parser.parse_args()

	# parser.print_help()
	print(args.models)
	print(args.output_dir)
	print(args.o)

	config = configparser.ConfigParser()
	# Read config
	config.read('config.ini')

	meshes = []
	# mesh_names = []

	for path in args.models:
		print(path)
		mesh_name = os.path.basename(path).split('.')[0]
		print("mesh name: " + mesh_name)
		# mesh_names.append(mesh_name)

		vert_lines, face_lines = read_lines_from_file(path)
		vertex_count = len(vert_lines)
		face_count = len(face_lines)

		# header_file_content = get_header_file_content(config, mesh_name, vertex_count, face_count)
		# source_file_content = get_source_file_content(config, header_filename, mesh_name, vert_lines, face_lines)
		# ic(source_file_content)

		use_fixed_point = config['DEFAULT'].getboolean('UseFixedPoint')
	
		if use_fixed_point:
			current_config_section = config['UseFixedPoint']
		else:
			current_config_section = config['UseFloatingPoint']

		vert_array_str, vert_array = get_vertex_array(current_config_section, mesh_name, vert_lines)
		face_array_str, face_array = get_face_array(current_config_section, mesh_name, face_lines)
		edge_array_str, edge_flags_str, *raw_arrays = get_edge_array(current_config_section, mesh_name, vert_array, face_array)

		edge_list = raw_arrays[0]
		edge_flags = raw_arrays[1]

		# ic(vert_array)
		# ic(face_array)
		# ic(edge_array_str)
		# ic(edge_flags_str)
		# ic(edge_list)
		# ic(edge_flags)

		mesh = Mesh(mesh_name, vert_array, face_array, edge_list, edge_flags)

		meshes.append(mesh)

		# print(mesh)
		# ic(mesh.edge_array)
	
	source_file_content = get_source_file_content(config, args.o, meshes)
	print(source_file_content)

	header_filename = args.o + '.h'
	source_filename = args.o + '.c'

	# for edge in edge_list:
	# 	s += f'\t{edge.v1_id}, {edge.v2_id}, {edge.face_id}'


	# header_file_content = get_header_file_content(config, mesh_name, vertex_count, face_count)
	# # ic(header_file_content)

	# source_file_content = get_source_file_content(config, header_filename, mesh_name, vert_lines, face_lines)
	# # ic(source_file_content)

	# with open(os.path.join(out_dir_path, header_filename), 'w') as header_file:
	# 	header_file.write(header_file_content)

	# with open(os.path.join(out_dir_path, source_filename), 'w') as source_file:
	# 	source_file.write(source_file_content)

if __name__ == "__main__":
	main()
