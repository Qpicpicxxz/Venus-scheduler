with open('../src/mem.S', 'r') as f:
    lines = f.readlines()

output = []
ignore = False

for line in lines:
    if '# python start jacklight' in line:
        ignore = True
    elif '# python end jacklight' in line:
        ignore = False
        output.append(line)
    elif not ignore:
        output.append(line)

with open('../src/mem.S', 'w') as f:
    f.writelines(output)

