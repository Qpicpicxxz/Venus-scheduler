import os

# Get the list of C files in the current directory
c_files = [f for f in os.listdir('.') if f.endswith('.c') and not f.endswith('_bin.c')]

# Loop over the C files and extract the task name from the file name
for c_file in c_files:
    task_name = os.path.splitext(c_file)[0]
    
    jacklight_string = f"# python start jacklight"

    # Construct the string to insert into os.ld
    insert_string = f"\n\t.{task_name} :{{\n\t\tPROVIDE(_{task_name}_start = .);\n\t\t*(.{task_name})\n\t\tPROVIDE(_{task_name}_end = .);\n\t}} >ram\n"

    # Open os.ld and read its contents
    with open('../os.ld', 'r') as f:
        contents = f.read()

    # Check if the insert string already exists in os.ld
    if insert_string not in contents:
        # Find the location to insert the new string
        insert_location = contents.find('# python end jacklight')

        # Insert the new string into os.ld
        new_contents = contents[:insert_location] + jacklight_string + insert_string + contents[insert_location:]

        # Write the modified contents back to os.ld
        with open('../os.ld', 'w') as f:
            f.write(new_contents)
