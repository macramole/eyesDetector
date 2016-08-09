#!/usr/bin/env python2
# coding=utf-8

import os
import sys
import glob
import subprocess

if len(sys.argv) < 2 :
	print "convierte el archivo pasado como parámetro y lo renombra al último dígito"
	print "usage: flv2mp4 movie_file"
	exit()

maxNumber = 0

for file in glob.glob("*"):
	splitted = file.split(".")
	if ( len(splitted) == 2 ):
		if ( splitted[0].isdigit() ):
			if ( int(splitted[0]) > maxNumber ):
				maxNumber = int(splitted[0])

maxNumber += 1
outputFile = str(maxNumber) + ".mp4"

result = subprocess.call(["avconv","-i",sys.argv[1],"-c:v","libx264","-an", outputFile])
if result == 0:
	os.remove(sys.argv[1])
	print "All done :)"
else:
	print "Hubo un error cuando se encodeaba"
