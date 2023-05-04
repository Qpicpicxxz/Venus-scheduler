import os

c_files = [f for f in os.listdir('./code') if f.endswith('.c') and not f.endswith('_bin.c')]

for c_file in c_files:
    task_name = os.path.splitext(c_file)[0]
    TASK_NAME = TASK_NAME = task_name.upper()
    
    jacklight_string = f"# python start jacklight"

    insert_string =  f"\n\t.{task_name} ALIGN(8):{{\n"
    insert_string += f"\t\tPROVIDE(_{task_name}_start = ALIGN(8));\n"
    insert_string += f"\t\t*(.{task_name})\n"
    insert_string += f"\t\tPROVIDE(_{task_name}_end = .);\n"
    insert_string += f"\t}} >ram\n"
    
    with open('../os.ld', 'r') as f:
        contents = f.read()

    if insert_string not in contents:
        insert_location = contents.find('# python end jacklight')

        new_contents = contents[:insert_location] + jacklight_string + insert_string + contents[insert_location:]

        with open('../os.ld', 'w') as f:
            f.write(new_contents)

