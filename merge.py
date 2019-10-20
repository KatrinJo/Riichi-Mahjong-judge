files = ['Mahjong1009.cpp', 'Tools.cpp', 'Yizhong.cpp', 'Player.cpp']
header = ['mahjong.h']

doc = ''
for h in header:
    doc += open(h, 'r', encoding='UTF-8').read() + '\n'
for file in files:
    doc += open(file, 'r', encoding='UTF-8').read() + '\n'

import re

for h in header:
    doc = re.sub('#include "%s"' % h, '', doc)

print(doc)