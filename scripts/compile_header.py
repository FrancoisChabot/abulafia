from __future__ import print_function

import argparse
import os
import sys


parser = argparse.ArgumentParser(description='Compiles a header by #including all local files.')
parser.add_argument('--output', help='where to save the result', required=True)
parser.add_argument('--input', help='the root header to load', required=True)
args = parser.parse_args()

loaded_files = set()
includes = set()

copyright_notice = '''//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)'''

header_guard = 'ABULAFIA_SINGLE_INCLUDE_H_'

def process(file):
  result = []
  if file in loaded_files:
    return result

  loaded_files.add(file)
  print('loading: {}'.format(file))

  with open(file) as f:
    raw = [x.rstrip() for x in f]
    
    # Remove comment-only lines and empty lines
    processed = []
    for r  in raw:
      s = r.strip()
      if not r or r.startswith(r'//'):
        continue

      # We store the raw string because we want to maintain indentation.
      processed.append(r)

    # Remove Header guards
    if (len(processed) >= 3 and 
        processed[0].strip().startswith('#ifndef') and
        processed[1].strip().startswith('#define') and
        processed[-1].strip().startswith('#endif')):

      processed = processed[2:-1]

    done_with_includes = False
    for line in processed:
      stripped = line.strip()
      if stripped.startswith("#include"):
        if done_with_includes:
          raise ValueError('There is an include after code has started')
        path = stripped[8:].strip()
        #all non-sysem includes belong in here.
        if path.startswith('"'):
          r = process(path[1:-1])
          result.extend(r)
        else:
          includes.add(line)
      else:
        done_with_includes = True
        result.append(line)

  result.append("")
  return result


result = process(args.input)

with open(args.output, 'w') as destination:
  print(copyright_notice, file=destination)
  print('#ifndef {}'.format(header_guard), file=destination)
  print('#define {}'.format(header_guard), file=destination)
  
  for h in sorted(includes):
    print(h, file=destination)
  
  print('', file=destination)

  for r in result:
    print(r, file=destination)

  print('#endif', file=destination)