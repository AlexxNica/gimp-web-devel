#!/bin/sh

rsync -rlt --delete --exclude-from=install.exclude ./ $1
