#!/bin/sh

THUMB=`echo $1 | sed s/\.[a-zA-Z0-9]*$//`

WIDTH=`identify -format %w $1`
HEIGHT=`identify -format %h $1`

if [ $WIDTH -gt $HEIGHT ]; then
    SIZE=256x
else
    SIZE=x256
fi

echo "$1 -> $THUMB-thumb.png"
convert -resize $SIZE $1 $THUMB-thumb.png

