import sys
with open(sys.argv[1], 'rb') as f:
    binary_data = f.read()

new_array = []

for i in range(0, len(binary_data), 4):
    sub_array = binary_data[i:i+4]
    sub_array = sub_array[::-1]
    new_value = int.from_bytes(sub_array, byteorder='big')
    new_array.append(new_value)
    # print(new_array)

with open(sys.argv[2], 'w') as f:
    f.write('unsigned int test_code[] __attribute__((section(".task3"))) = {')
    for value in new_array:
        f.write(hex(value) + ', ')
    f.write('};\n')
