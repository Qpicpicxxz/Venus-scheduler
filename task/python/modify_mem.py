import os

c_files = [f for f in os.listdir('./code') if f.endswith('.c') and not f.endswith('_bin.c')]

for c_file in c_files:
    task_name = os.path.splitext(c_file)[0]
    TASK_NAME = task_name.upper()
    
    jacklight_string = f"# python start jacklight"

    insert_string = f"\n.global {TASK_NAME}_START\n"
    insert_string += f"{TASK_NAME}_START: .word _{task_name}_start\n"
    insert_string += f"\n.global {TASK_NAME}_END\n"
    insert_string += f"{TASK_NAME}_END: .word _{task_name}_end\n"
    insert_string += f"\n.global {TASK_NAME}_LEN\n"
    insert_string += f"{TASK_NAME}_LEN: .word _{task_name}_end - _{task_name}_start\n\n"
    
    with open('../src/mem.S', 'r') as f:
        contents = f.read()

    if insert_string not in contents:
        insert_location = contents.find('# python end jacklight')

        new_contents = contents[:insert_location] + jacklight_string + insert_string + contents[insert_location:]

        with open('../src/mem.S', 'w') as f:
            f.write(new_contents)

