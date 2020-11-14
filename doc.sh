#!/bin/bash

DOC_DIR=public/doc/
REFS="
master
"

IFS='
'

mkdir -p $DOC_DIR

for REF in $REFS; do
  API_VERSION=`echo $REF | sed 's/retro-gtk-\([0-9][0-9]*\)-\([0-9][0-9]*\)/\1.\2/'`

  curl -L --output "$REF.zip" "https://gitlab.gnome.org/GNOME/retro-gtk/-/jobs/artifacts/$REF/download?job=reference"
  unzip -d "$REF" "$REF.zip"
  mv "$REF/_reference" $DOC_DIR/$API_VERSION

  rm "$REF.zip"
  rm -rf "$REF"
done
