cat * | avconv -f image2pipe -c:v mjpeg -i - -r 30 -map 0 out.mov
avconv -i out.mov -filter:v "setpts=2.0*PTS" out2.mov
