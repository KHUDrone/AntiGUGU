# -*- coding: utf-8 -*-
"""
Created on Sat Aug 14 01:01 2021

@author: Mason EungChang Lee
"""

import sys
import signal

def signal_handler(signal, frame): # ctrl + c -> exit program
        print('You pressed Ctrl+C!')
        sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)


def output_name_and_pose_from_world(world):
    file_ = open(world, 'r')
    lines = file_.readlines()
    file_.close()

    idx = []
    temp = 0
    for i, line in enumerate(lines):
        if 'pose frame=' in line:
            if '0 0 0' not in line and '          ' not in line: 
                idx.append(i)

    count = 0
    for j in idx:
        print("    <include>")
        print("      <name>d%d</name>"%count)
        print(lines[j]+"      <uri>model://dbrick</uri>")
        print("    </include>")
        count = count+1

if __name__ == '__main__':
    output_name_and_pose_from_world('brick.world')
