#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Jul  5 23:46:17 2020

@author: ashlan

Analyze output files for GEANT4 Simulation.

Function made for csv formatted with 4 columns. 
If more or less columns, change the the columns used.
"""

import numpy as np
import re
import os
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


def make_photon_arrays(path, numevents):
    
    """ Make array of nSiPMPhotons and nAbsPhotons
    from the csv output file of GEANT4.
    
    Parameters:
        path: Path to the folder with data files in it.
    
        numevents: Number of events for each run.
               
    Returns:
        xcoords: X-coordinates in same order of arrays since
                 files are not processed linearly.
        
        zcoords: Z-coordinates in same order of arrays.
        
        nsipmarrays: List of the nSiPMPhoton arrays for each run.
        
        nabsarrays: List of the nAbsPhoton arrays for each run.
        """
    xcoords = []
    zcoords = []
    
    nsipmarrays = []
    nabsarrays = []
    
    for filename in os.listdir(path):

        photondata = np.loadtxt(path+'/'+filename,delimiter=',',usecols=[1,4])

        coords = filename[0:8]

        arraylen = len(photondata.flatten('F'))
        
        nsipmphotons = photondata.flatten('F')[numevents:arraylen]
        #print(len(nsipmphotons))
        nabsphotons = photondata.flatten('F')[0:numevents] 
        
        nsipmarrays.append(nsipmphotons)
        nabsarrays.append(nabsphotons)
        
        x = re.findall('(-[0-9]+)x',coords) 
        
        if bool(x) == False:
            x = re.findall('([0-9]+)x', coords)
       
        z = re.findall('(-[0-9]+)z',coords)  

        if bool(z) == False:
            z = re.findall('([0-9]+)z',coords)

        xcoords.append(x[0])
        zcoords.append(z[0])
    
    xcoords = np.array(xcoords).astype(np.float)
    zcoords = np.array(zcoords).astype(np.float)
    
    return xcoords, zcoords, nsipmarrays, nabsarrays
 
path1 = './photondata'

arrs = make_photon_arrays(path1, 150)

nsipmave = np.array([np.average(array) for array in arrs[2]])
nabsave = np.array([np.average(array) for array in arrs[3]])

nsipmplot = np.zeros((11,11))

original_xaxis = np.arange(-25,30,5)
arrayaxis = np.arange(0,11)

original_zaxis = np.arange(25,-30, -5)

x_axis_map = dict(zip(original_xaxis, arrayaxis))
z_axis_map = dict(zip(original_xaxis, arrayaxis))

for i in range(0, 120):
    x = x_axis_map[arrs[0][i]]
    z = z_axis_map[arrs[1][i]]
    
    nsipmplot[x,z] = nsipmave[i]

print(nsipmplot)

fig, ax = plt.subplots()
plot = ax.imshow(nsipmplot, cmap='plasma')
plt.xticks(arrayaxis,original_xaxis)
plt.yticks(arrayaxis,original_zaxis)
plt.colorbar(plot)
plt.show()







