#!/bin/sh

BASE=`echo $1 | sed s/\.[a-zA-Z0-9]*$//`

SIZE="256x256"

echo "$1 -> $BASE-thumb.png"
convert -resize $SIZE $1 $BASE-thumb.png
