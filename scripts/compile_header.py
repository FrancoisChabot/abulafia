import argparse
import os
import sys

parser = argparse.ArgumentParser(description='Compiles a header by #including all local files.')
parser.add_argument('--output', help='where to save the result', required=True)
parser.add_argument('--input', help='the root header to load', required=True)
args = parser.parse_args()

loaded_files = set()


def process(file, dst):
  if file in loaded_files:
    return ""
  loaded_files.add(file)
  print("loading: " + file)

  with open(file) as f:
    for line in f:
      stripped_line = line.strip()
      if not stripped_line:
        continue
      if stripped_line.startswith("#include"):
        path = stripped_line[8:].strip()
        #all non-sysem includes belong in here.
        if path.startswith('"'):
          process(path[1:-1], dst)
          continue
      print(line.rstrip(), file=dst)


with open(args.output, "w") as destination:
  process(args.input, destination)
