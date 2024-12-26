import re
import os
import argparse
from collections import OrderedDict

magic_line = re.compile(r"LBR dump: (\d+) records")
NUM_SAMPLES = 8 # every 8 lines form a sample

def parse_log(log_file_path):
  with open(log_file_path, "r") as log_file:
    found_magic = False
    tmp_tuple_list = []
    range_map = OrderedDict()
    branch_map = OrderedDict()
    for line in log_file:
      magic_line_match = magic_line.match(line)
      # finding the starting line
      if magic_line_match:
        found_magic = True
        num_records = int(magic_line_match.group(1))
        assert num_records % NUM_SAMPLES == 0
      # iteration assumes continuous records
      if found_magic:
        for _ in range(num_records):
          line = next(log_file)
          tmp_tuple_list.append(tuple(map(lambda x: int(x, 16) - 0x80000000, line.strip().split(','))))
        # process the sample
        for i in range(0, num_records, NUM_SAMPLES):
          for j in range(i, i + NUM_SAMPLES):
            branch_map[tmp_tuple_list[j]] = branch_map.get(tmp_tuple_list[j], 0) + 1
            if j % NUM_SAMPLES != NUM_SAMPLES - 1:
              # range is from the last branch dst to the current branch src
              range_tuple = (tmp_tuple_list[j+1][1], tmp_tuple_list[j][0])
              range_map[range_tuple] = range_map.get(range_tuple, 0) + 1
        return range_map, branch_map
    
def write_results(range_map, branch_map):
    # write the results
    with open("lbr_branch.txt", "w") as branch_file:
      branch_file.write(f"{len(range_map)}\n")
      for k, v in range_map.items():
        branch_file.write(f"{k[0]}-{k[1]}:{v}\n")
      branch_file.write("0\n")
      branch_file.write(f"{len(branch_map)}\n")
      for k, v in branch_map.items():
        branch_file.write(f"{k[0]}->{k[1]}:{v}\n")

if __name__ == "__main__":
  parser = argparse.ArgumentParser(description="Parse spike log and write lbr files")
  parser.add_argument("log_file", help="Path to the vcs log file")
  args = parser.parse_args()
  range_map, branch_map = parse_log(args.log_file)
  write_results(range_map, branch_map)
