import re
import os

import argparse

# Regular expressions to match file path and hex data lines
file_path_pattern = re.compile(r"^Emitting \d+ bytes for (.+)$")
hex_line_pattern = re.compile(r"^([0-9a-f]{8}):((?: [0-9a-f]{2})+)")

def parse_log_and_write_dump(log_file_path):
	with open(log_file_path, "r") as log_file:
		current_file = None
		hex_data = []
    
		for line in log_file:
			# Check for the start of a new section with a file path
			file_path_match = file_path_pattern.match(line)
			if file_path_match:
				# Write any previous hex data to its file if there was a previous file section
				if current_file and hex_data:
					write_hex_to_file(current_file, hex_data)
				# Start a new file section
				current_file = file_path_match.group(1)
				hex_data = []
				continue

			# Check if the line contains hex data
			hex_line_match = hex_line_pattern.match(line)
			if hex_line_match:
				# Add hex bytes to the hex_data list (ignoring the address part)
				hex_bytes = hex_line_match.group(2).strip().split()
				hex_data.extend(hex_bytes)

		# Write the last file data if it exists
		if current_file and hex_data:
			write_hex_to_file(current_file, hex_data)

def write_hex_to_file(file_path, hex_data):
    # Create directory if it does not exist
    os.makedirs(os.path.dirname(file_path), exist_ok=True)
    
    # Convert hex data to binary and write to file
    with open(file_path, 'wb') as binary_file:
        binary_file.write(bytearray(int(byte, 16) for byte in hex_data))
    print(f"Written data to {file_path}")
		
if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Parse gcov log and write gcda files")
	parser.add_argument("log_file", help="Path to the gcov log file")
	args = parser.parse_args()
	parse_log_and_write_dump(args.log_file)
