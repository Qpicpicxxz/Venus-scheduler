# this python script is for binary to c converting
# 1. read binary files from command line (task.bin)
# 2. catch the binary data and convert it to instructions (0x00000000)
# 3. rewrite this data to a c file as a certain format

import sys
import os

args = sys.argv[1:]
for arg in args:
  task_bin_file = arg
  task_bin = os.path.basename(arg)
  task = task_bin.replace('.bin','')
  name,ext=task_bin.split('.')
  task_output = './cbin/' + name + '_' + ext + '.c'
  print("  "+task_output+' is generated')
  with open(task_bin_file, 'rb') as f:
      binary_data = f.read()

  new_array = []

  for i in range(0, len(binary_data), 4):
      sub_array = binary_data[i:i+4]
      sub_array = sub_array[::-1]
      new_value = int.from_bytes(sub_array, byteorder='big')
      new_array.append(new_value)


  with open(task_output, 'w') as f:
      f.write('unsigned int ')
      f.write(task)
      f.write('_bin[] __attribute__((section(".')
      f.write(task)
      f.write('"))) = {')
      for value in new_array:
          f.write(hex(value) + ',\n')
      f.write('};\n')

