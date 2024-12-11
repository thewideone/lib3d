"""
Before use, type ".\.venv\Scripts\activate"
to activate Python's virtual environment
"""

import sys
import os
import configparser
import numpy as np
from icecream import ic

# Save config
# with open('config.ini', 'w') as configfile:
# 	config.write(configfile)

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

def get_vertex_array(config, vert_lines, mesh_name) -> str:
	"""
	Get C-style array with vertices data
	"""

	def get_vertex_array_str() -> str:
		"""
		Compose a string with the content of C-style array with vertex data
		"""
		vertex_count = len(vert_lines)

		# ic(vert_lines)
		# ic(vertex_count)

		if vertex_count == 0:
			print("Error: no vertices in the input file")
			return ''
		
		result = ""

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

				values = np.array([], dtype=fixed_point_type)

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
						return ''
					
					values = np.append( values, str(value) )

				result += '\t' +  ', '.join(values)

			else:
				result += '\t' + ', '.join( elements[1:] )

			if line != last_vert_line:
				result += ',\n'
			else:
				result += '\n'
		
		# ic(result)

		return result
	
	vertex_array_type = config['VertexArrayType']

	result = "const " + vertex_array_type + " " + mesh_name + "_mesh_verts[] = {\n"
	result += get_vertex_array_str()
	result += "};\n"

	return result

def get_face_array(config, face_lines, mesh_name) -> str:
	"""
	Get C-style array with faces data
	"""

	def get_face_array_str() -> str:
		"""
		Compose a string with the content of C-style array with face data
		"""

		face_count = len(face_lines)

		if face_count == 0:
			print("Error: no faces in the input file")
			return ''

		last_face_line = face_lines[-1]

		# ic(face_lines)
		# ic(face_count)

		result = ''

		for line in face_lines:
			elements = line.split()

			# Subtract 1 from each vertex ID since indices in C start from 0
			elements[1:] = [*map( lambda x: str(int(x)-1), elements[1:] )]
			
			result += '\t' + ', '.join( elements[1:] )

			if line != last_face_line:
				result += ',\n'
			else:
				result += '\n'

		return result

	face_array_type = config['FaceArrayType']

	result = "const " + face_array_type + " " + mesh_name + "_mesh_faces[] = {\n"
	result += get_face_array_str()
	result += "};\n"

	return result

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
	s = "#define " + mesh_name.upper() + "_MESH_VERT_COUNT " + str(vertex_count) + "\n"
	s += "#define " + mesh_name.upper() + "_MESH_FACE_COUNT " + str(face_count) + "\n"

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
	
	header_def_symbol = "_" + mesh_name.upper() + "_H_"
	s = ''

	s += "#ifndef " + header_def_symbol + '\n'
	s += "#define " + header_def_symbol + '\n'
	s += '\n'
	s += get_header_comment(current_config_section, mesh_name)
	s += '\n'
	s += get_defines(mesh_name, vertex_count, face_count)
	s += '\n'
	s += "extern const " + config['DEFAULT']['RationalType'] + " " + mesh_name + "_mesh_verts[];\n"
	s += "extern const " + config['DEFAULT']['FaceArrayType'] + " " + mesh_name + "_mesh_faces[];\n"
	s += "#endif // " + header_def_symbol + '\n'

	
	# vertex_array_fixed = get_vertex_array(fixed_point_config, vert_lines, mesh_name)
	# vertex_array_floating = get_vertex_array(floating_point_config, vert_lines, mesh_name)
	# face_array = get_face_array(fixed_point_config, face_lines, mesh_name)

	# ic(defines)
	# ic(vertex_array_fixed)
	# ic(vertex_array_floating)
	# ic(face_array)

	ic(s)

	return s

def get_source_file_content(config, mesh_name, vert_lines, face_lines) -> str:
	"""
	Generate content of the output source file
	"""

	use_fixed_point = config['DEFAULT'].getboolean('UseFixedPoint')
	
	if use_fixed_point:
		current_config_section = config['UseFixedPoint']
	else:
		current_config_section = config['UseFloatingPoint']

	s = ''
	s += '#include "' + mesh_name + '.h"\n'
	s += '\n'
	s += get_vertex_array(current_config_section, vert_lines, mesh_name)
	s += '\n'
	s += get_face_array(current_config_section, face_lines, mesh_name)
	s += '\n'

	return s

def main() -> None:
	"""
	Read input file, perform calculations and save results in output files.
	"""

	if len(sys.argv) < 3:
		print("Usage: python model_parser.py <input_file_path> <output_directory_path>")
		return

	#for arg in sys.argv:
		#print(arg)

	in_file_path = sys.argv[1]
	out_file_path = sys.argv[2]

	if not os.path.exists(in_file_path):
		print("Error: invalid input file path")
		return

	if not os.path.exists(out_file_path):
		print("Error: invalid output file path")
		return

	mesh_name = os.path.basename(in_file_path).split('.')[0]
	print("mesh name: " + mesh_name)

	vert_lines, face_lines = read_lines_from_file(in_file_path)
	vertex_count = len(vert_lines)
	face_count = len(face_lines)
	
	config = configparser.ConfigParser()

	# Read config
	config.read('config.ini')
	

	header_file = mesh_name + '.h'
	source_file = mesh_name + '.c'

	header_file_content = get_header_file_content(config, mesh_name, vertex_count, face_count)
	ic(header_file_content)

	source_file_content = get_source_file_content(config, mesh_name, vert_lines, face_lines)
	ic(source_file_content)


if __name__ == "__main__":
	main()
