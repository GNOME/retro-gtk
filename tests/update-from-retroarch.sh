#!/bin/sh

function die() {
  echo $*
  exit 1
}

if test -z "$DIR"; then
  echo "Must set DIR"
  exit 1
fi

if test -z "$SRCDIR"; then
  echo "Must set SRCDIR"
  exit 1
fi

if test -z "$FILES"; then
  echo "Must set FILES"
  exit 1
fi

cd "$SRCDIR"

for FILE in $FILES; do
  TMPFILE=`mktemp`
  sed 's/[ \t]*$//' "$DIR/$FILE" > $TMPFILE
  if cmp -s $TMPFILE $FILE; then
    echo "File $FILE is unchanged"
  else
    cp $TMPFILE $FILE || die "Could not move $DIR/$FILE to $FILE"
    echo "Updated $FILE"
    git add $FILE
  fi
done
