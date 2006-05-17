#!/bin/sh

# force rebuild of index.html and changelog.html to update RSS feeds
rm index.html
rm changelog.html
make

rsync -rlt --delete --exclude-from=install.exclude ./ $1
