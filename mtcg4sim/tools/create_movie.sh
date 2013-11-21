#!/bin/sh
ls -v1 | egrep [0-9]+ > jpg_list.txt
#FILE_NAMES=$(ls -v1 | egrep [0-9]+ | tr '\n' ',' | sed 's:,$:\n:')
#mencoder mf://@jpg_list.txt -mf w=800:h=600:fps=25:type=jpg \
#    -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o output.avi
convert 
rm jpg_list.txt
