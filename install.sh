#!/bin/sh

# need to set PATH here to get a recent version of xsltproc
PATH=/home/neo/bin make

rsync -rlt --delete --exclude-from=install.exclude ./ $1
