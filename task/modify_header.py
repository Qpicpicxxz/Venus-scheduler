import os

c_files = [f for f in os.listdir('.') if f.endswith('.c') and not f.endswith('_bin.c')]

for c_file in c_files:
    task_name = os.path.splitext(c_file)[0]
    TASK_NAME = TASK_NAME = task_name.upper()
    
    jacklight_string = f"// python start jacklight"

    insert_string =  f"\nextern uint32_t {TASK_NAME}_START;"
    insert_string += f"\nextern uint32_t {TASK_NAME}_END;"
    insert_string += f"\nextern uint32_t {TASK_NAME}_LEN;\n"
    
    with open('../include/codeaddr.h', 'r') as f:
        contents = f.read()

    if insert_string not in contents:
        insert_location = contents.find('// python end jacklight')

        new_contents = contents[:insert_location] + jacklight_string + insert_string + contents[insert_location:]

        with open('../include/codeaddr.h', 'w') as f:
            f.write(new_contents)
