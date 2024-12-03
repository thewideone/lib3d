"""
Before use, type ".\.venv\Scripts\activate"
to activate Python's virtual environment
"""

import sys
import os
import configparser
import numpy as np

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

def get_header_comment(config, mesh_name) -> str:
	"""
	Generate header comment
	"""

	s =	 "// \n"
	s += "// " + config['DEFAULT']['InfoGeneratedBy'] + "\n"
	s += "// Mesh name: '" + mesh_name + "'\n"
	if config['DEFAULT']['UseFixedPoint'] is True:
		s += "// Fixed point type: " + config['DEFAULT']['FixedPointType'] + "\n"
		s += "// Fixed point binary digits: " + config['DEFAULT']['FixedPointBinaryDigits'] + "\n"
	s += "// Vertex array type: " + config['DEFAULT']['VertexArrayType'] + "\n"
	s += "// Face array type: " + config['DEFAULT']['FaceArrayType'] + "\n"
	s += "// \n"
	return s


def main() -> None:
	"""
	Read input file, perform calculations and save results in output files.
	"""

	for arg in sys.argv:
		print(arg)

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

	if vertex_count == 0 or face_count == 0:
		print("Error: no vertices or faces in the input file")
		return

	last_vert_line = vert_lines[-1]
	last_face_line = face_lines[-1]

	config = configparser.ConfigParser()

	# Read config
	config.read('config.ini')

	header_file = get_header_comment(config, mesh_name)
	print(header_file)

	source_file = ""



if __name__ == "__main__":
	main()
