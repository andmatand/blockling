#!/bin/sh
DATADIR=usr/local/games/blockling

# Compile the executable and then come back to this directory
cd ../..
make DATADIR=$DATADIR/
mipsel-linux-strip blockling
cd -

# Create a temp directory with the necessary package files
TEMPDIR=package
mkdir $TEMPDIR
cp default.gcw0.desktop $TEMPDIR
cp icon.png $TEMPDIR
cp ../../blockling $TEMPDIR
cp ../../README.md $TEMPDIR
mkdir -p $TEMPDIR/$DATADIR
rsync -av --exclude='*.xcf' ../../data/* $TEMPDIR/$DATADIR

# Create the .opk package
mksquashfs $TEMPDIR blockling.opk -all-root -noappend -no-exports -no-xattrs

# Delete the temp directory
rm -rf $TEMPDIR
