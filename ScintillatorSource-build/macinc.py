#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jun 24 17:08:46 2020

@author: ashlan

Increment .mac files while runs happening
"""

import sys
import os

spacing = 1 #mm

"""Opens mac file and loads up the lines"""
with open('muplus.mac', 'r') as macfile:
    maclines = macfile.readlines()
    macfile.close()

#print(maclines[3])

"""Loads up the positions line and selects the x and z coords"""
pos = maclines[3].split()

x = int(pos[1])

z = int(pos[3])

maclines[4] = '/analysis/setFileName {}x{}zphotoncnt\n'.format(x,z) 

"""Checks if it has reached other corner of puck and exits 
if it has"""
if z == 25 and x == 25:
    os.system("killall autorun.sh")
    sys.exit("ROOT files created. Exiting...")

"""Increments z by 1 when x reaches the top of puck
then restarts x to go from -25 to 25 again."""
if x == 25:
    z = z + spacing
    x = -25 - spacing

x = x + spacing

if x == 0 and z == 0:
    os.system("echo 50% Complete...")
"""Writes the line to the list of lines"""
maclines[3] = '/gps/pos/centre  {} 100. {} mm\n'.format(x,z)

#print(maclines[3])

"""Erases the textfile and rewrites it with edits."""
with open('muplus.mac','w') as macedit:
    macedit.writelines(maclines)
    macedit.close()


    
    

