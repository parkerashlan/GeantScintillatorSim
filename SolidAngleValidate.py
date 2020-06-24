#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jun  1 16:02:23 2020

@author: ashlanparker
"""

import numpy as np

xpos = float(input("Please input x position of particle (mm): "))

if xpos > 24.99 or xpos < -24.99:
    raise ValueError("Particle outside of scintillator!")

xpos = xpos - 25
    
solidangle = (9/(4*np.pi*(xpos**2)))*1.053e6

"""9 is the surface area of the SiPM, 4pi*dist**2 is the surface area
of the sphere, and 1.043e6 is about how many photons are produced each time.
Can change this number to get a more accurate estimate."""


print("Estimated number of photons absorbed: ",solidangle)