#!/bin/sh
video=$1
if [ -z $video ]; then
	video="1.avi"
fi
make && bin/eyesDetector $video
