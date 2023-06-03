# -*- coding: utf-8 -*-
"""
Created on Sat Aug 14 01:01 2021

@author: Mason EungChang Lee
"""

import random
import sys
import signal

def signal_handler(signal, frame): # ctrl + c -> exit program
        print('You pressed Ctrl+C!')
        sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)


def output_name_and_pose_from_world(sdf):
    file_ = open(sdf, 'r')
    lines = file_.readlines()
    file_.close()

    file_old = open('backup_'+sdf, 'w')
    file_old.writelines(lines)

    idx = []
    for i, line in enumerate(lines):
        if 'pose frame=' in line:
            idx.append(i)

    for k in idx:
        a=lines[k].split(' ')
        temp = 0
        for ele in a:
            if 'pose' in ele:
                if '/pose' not in ele:
                    tempidx = a.index(ele)+3
                    a[tempidx] = str(float(a[tempidx])+0.25*random.randrange(1,11))
        lines[k] = " ".join(a)

    count = 0
    for j in idx:
        if j%2==0:
            print("    <include>")
            print("      <name>d%d</name>"%count)
            print(lines[j]+"      <uri>model://dbrick</uri>")
            print("    </include>")
            count = count+1
#    file_new = open(world, 'w')
#    file_new.writelines(lines)
#    file_new.close()
            

if __name__ == '__main__':
    output_name_and_pose_from_world('model.sdf')
