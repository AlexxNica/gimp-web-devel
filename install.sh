#!/bin/sh

# Maybe do a make here?
rsync -rlt --delete --exclude-from=install.exclude ./ $1
