#!/bin/sh

# need to set PATH here to get a recent version of xsltproc
export PATH=/home/neo/bin:$PATH

# force rebuild of changelog.html to update RSS feed
rm changelog.xml
make

# seems to fix a bug somewhere else
# chmod 644 changelog.html

rsync -rlt --delete --exclude-from=install.exclude ./ $1
