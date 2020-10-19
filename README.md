# GEANT4 Scintillator-SiPM Simulation
This is an undergraduate research project done during Spring 2020 under David Stuart of UCSB. The goal of the project was to explore simulation by utilizing the software toolkit of GEANT4.
I aimed to see if I could determine the energy of the cosmic ray based on the amount of photons which hit the silicon photomultiplier (SiPM) on the side of the scintillator
puck or cube. I also carried out other tests including creating a heat map of the face of the puck based on the mean number of photons which hit the SiPM and
the amount of photons which hit the SiPM when the angle from the zenith is varied.

### Visualization of the system

![alt text](https://github.com/parkerashlan/GeantScintillatorSim/blob/master/geant.png?raw=true)

The red cube is the teflon wrapper, the green cube is the scintillator made of BC408,the blue cube on the side is the SiPM, and the yellow shape is the back of the SiPM to prevent reflections.

### Parameters of the simulation

Materials:
* Scintillator = BC408
* SiPM = Silicon
* Wrapper = Teflon
* Atmosphere = Air

Particle Data:
* Cosmic Rays = Muons
* Muon Energy = 4-10 GeV

Particles are shot from 10 cm away from the puck.

