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

def get_vertex_array_str(config, vert_lines):
	"""
	Compose a string with the content of C-style array with vertex data
	"""
	vertex_count = len(vert_lines)

	# ic(vert_lines)
	# ic(vertex_count)

	if vertex_count == 0:
		print("Error: no vertices in the input file")
		return
	
	result = ""

	last_vert_line = vert_lines[-1]

	for line in vert_lines:
		elements = line.split()

		if config.getboolean('UseFixedPoint'):
			# Convert floating point values into fixed point ones
			fixed_point_type = config['FixedPointType']
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
					return
				
				values = np.append( values, str(value) )

			result += '\t' +  ', '.join(values)

			if line != last_vert_line:
				result += ',\n'
			else:
				result += '\n'
	
	# ic(result)

	return result


def get_face_array_str(config, face_lines):
	"""
	Compose a string with the content of C-style array with face data
	"""

	face_count = len(face_lines)

	if face_count == 0:
		print("Error: no faces in the input file")
		return

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

def get_header_comment(config, mesh_name) -> str:
	"""
	Generate header comment
	"""

	s =	 "// \n"
	s += "// " + config['InfoGeneratedBy'] + "\n"
	s += "// Mesh name: '" + mesh_name + "'\n"
	if config['UseFixedPoint'] is True:
		s += "// Fixed point type: " + config['FixedPointType'] + "\n"
		s += "// Fixed point binary digits: " + config['FixedPointBinaryDigits'] + "\n"
	s += "// Vertex array type: " + config['VertexArrayType'] + "\n"
	s += "// Face array type: " + config['FaceArrayType'] + "\n"
	s += "// \n"
	return s

def get_defines(mesh_name, vertex_count, face_count):
	"""

	"""
	s = "#define " + mesh_name.upper() + "_MESH_VERT_COUNT " + str(vertex_count) + "\n"
	s += "#define " + mesh_name.upper() + "_MESH_FACE_COUNT " + str(face_count) + "\n"

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

	

	config = configparser.ConfigParser()

	# Read config
	config.read('config.ini')
	fixed_point_config = config['UseFixedPoint']

	header_file = get_header_comment(fixed_point_config, mesh_name)
	print(header_file)

	source_file = "source .c file"

	vertex_array_content = get_vertex_array_str(fixed_point_config, vert_lines)

	face_array_content = get_face_array_str(fixed_point_config, face_lines)

	# print(result)
	



if __name__ == "__main__":
	main()
