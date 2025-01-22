import re
import os
import argparse

def parse_log(log_path):
    with open(log_path, "r") as log_file:
        lines = log_file.readlines()
        for idx, line in enumerate(lines):
            if count_match := re.match(r"\[l_trace_sink_dma_read\] count: (\d+)", line):
                count = int(count_match.group(1))
                target_line = lines[idx + 1].strip()
                split_line = target_line.split()
                assert len(split_line) == count
                return split_line
    return None

# write as raw hex dump
def write_results(split_line):
    with open("ltrace_dram_dump.txt", "wb") as dump_file:
        for hex_str in split_line:
            byte_val = int(hex_str, 16)
            dump_file.write(bytes([byte_val]))

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  parser.add_argument("--log_path", type=str, required=True)
  args = parser.parse_args()
  log_path = args.log_path
  split_line = parse_log(log_path)
  write_results(split_line)
