#!/bin/bash

# Check that private headers aren't included in public ones.
if grep "include.*private.h" $(ls retro-gtk/*.h shared/*.h | grep -v private);
then
  echo "Private headers shouldn't be included in public ones.";
  exit 1
fi

# Check that retro-gtk.h contains all the public headers.
for header in $(ls retro-gtk shared | grep \.h$ | grep -v private | grep -v retro-gtk.h);
do
  if ! grep -q "$(basename $header)" retro-gtk/retro-gtk.h;
  then
    echo "The public header" $(basename $header) "should be included in retro-gtk.h."
    exit 1
  fi
done
