import re

output = ''

regex = re.compile('(\\S+) = COLOR_CONSTRUCT\\((\\d+), (\\d+), (\\d+), (\\d+)\\), // (\\S+)')

with open('colors.txt') as file:
    for line in file:
        match = regex.match(line)
        groups = match.groups()
        r = float(groups[1]) / 255.0
        g = float(groups[2]) / 255.0
        b = float(groups[3]) / 255.0
        a = float(groups[4]) / 255.0
        output += f'.{groups[0]} = {{ {r}f, {g}f, {b}f, 1 }}, // {groups[5]}\n'

print(output)
        