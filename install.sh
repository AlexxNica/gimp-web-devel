#!/bin/sh

# need to set PATH here to get a recent version of xsltproc
export PATH=/home/neo/bin:$PATH

# force rebuild of changelog.html to update RSS feed
touch changelog.xml
make

rsync -rlt --delete --exclude-from=install.exclude ./ $1
