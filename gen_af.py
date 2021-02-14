#! /usr/bin/env python

table1 = []
mode = False

while True:
    line = raw_input()
    if line=="":
        mode = not mode
        continue
    if not mode:
        table1.append(line)
    else:
        print table1[0]+" : "+line+","
        del table1[0]
