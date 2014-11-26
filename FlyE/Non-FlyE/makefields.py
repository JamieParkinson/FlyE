#!/usr/bin/env python
# Runs LEXSIMECK for all layers and organises files
# Pass path to PA files as argument 1, path to the initial cfg file as
# argument 2, path for output directory as argument 3
# 06/2014 Jamie Parkinson

import sys
import os
import subprocess
import struct
import re

# Initialise variables
paPath = os.path.abspath(sys.argv[1])+'/' # PA files directory
paFiles = os.listdir(paPath) # List of PA files
paName = re.findall(r'\w+(?=.pa\d+)',paFiles[0])[0] # PA file basename
paFiles.remove(paName+".pa0") # Get rid of pa0 and pa#
paFiles.remove(paName+".pa#")
nElectrodes = len(paFiles)
cfgFile = os.path.abspath(sys.argv[2]) # Config file
outputDir = os.path.abspath(sys.argv[3])+'/' # Output directory
OLD_OUT_NAMES = [['mat_ga_x.dat','X'],['mat_ga_y.dat','Y'],['mat_ga_z.dat','Z']] # Default LEXSIMECK output names with dimension
(mode,symmetry,maxv,nx,ny,nz,mirror) = struct.unpack('=iidiiii',open(paPath+paFiles[2],'r').read(32)) # Gets the dimensions of the PA


def setInits(): # Sets the output and PA paths in the cfg file. Made as a function to keep vars local
	cfgIn = open(cfgFile,'r').read()
	cfgOut = open(cfgFile,'w')
	newCfgTxt = re.sub(r'(OUTPUT_PATH\=\s*)(.+?\s)',r'\g<1>'+outputDir+' ',cfgIn)
	newCfgTxt = re.sub(r'(PA_PATH\=\s*)(.+?\s)',r'\g<1>'+paPath+' ',newCfgTxt)
	cfgOut.write(newCfgTxt)
	return 1

def writeLayer(x): # Writes the value x to EXTRACTION_COORDINATE in the cfg file
	cfgIn = open(cfgFile,'r').read()
	cfgOut = open(cfgFile,'w')
	newCfgTxt = re.sub(r'(EXTRACTION_COORDINATE\=\s*)([0-9]+)',r'\g<1>'+str(x),cfgIn)
	cfgOut.write(newCfgTxt)
	return 1
	
def writePAname(name): # Writes the PA file name to PA_FILE_NAME in the cfg file
	cfgIn = open(cfgFile,'r').read()
	cfgOut = open(cfgFile,'w')
	newCfgTxt = re.sub(r'(PA_FILE_NAME\=\s*)([A-Za-z]+.pa[0-9]*)',r'\g<1>'+name,cfgIn)
	cfgOut.write(newCfgTxt)
	return 1
	
def writeFlyEconf():
	flyEconf = open(outputDir+'../flyE.conf','w')
	flyEconf.write('''N_ELECTRODES=\t%i
X_DIMENSION=\t%i
Y_DIMENSION=\t%i
Z_DIMENSION=\t%i
DAT_DIRECTORY=\t%s
PA_NAME=\t%s''' % (nElectrodes,nx,ny,nz,outputDir,paName))
	return 1
	
setInits()

for i,pa in enumerate(paFiles): # Loop over electrodes
	writePAname(pa)
	electrodeN = re.findall(r'[0-9]+',pa)[0]
	print "Working on electrode "+electrodeN+":"
	
	for x in range(2,nx): # Loop over layers
		writeLayer(x)
		print "Electrode %d/%d; Layer %d/%d - done" % (i,nElectrodes,x,nx-2)
		
		with open(os.devnull,'w') as fnull:
			subprocess.call("./LEXSIMECK.OUT "+cfgFile+" -q",shell=True,stdout=fnull,stderr=fnull)
		
		for oldname in OLD_OUT_NAMES: # Rename the x/y/z files
			# newname format: <PA name>_E<electrode number>_L<layer number>_<x|y|z>.dat
			newname = "%s%s_E%s_L%i_%s.dat" % (outputDir,paName,electrodeN,x,oldname[1])
			os.rename(outputDir+oldname[0], newname)
			print "\t\tRenamed "+oldname[0]+" to "+newname

os.remove(outputDir+"mat_ga_e.dat")
writeFlyEconf()
print "Done!"		
		
		
