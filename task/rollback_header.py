with open('../include/codeaddr.h', 'r') as f:
    lines = f.readlines()

output = []
ignore = False

for line in lines:
    if '// python start jacklight' in line:
        ignore = True
    elif '// python end jacklight' in line:
        ignore = False
        output.append(line)
    elif not ignore:
        output.append(line)

with open('../include/codeaddr.h', 'w') as f:
    f.writelines(output)
