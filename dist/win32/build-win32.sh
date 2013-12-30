#!/bin/bash
# This script will create a distributable zip file of blockling for Windows

if [ $# -lt 1 ]; then
    echo "usage: $0 <path/to/MXE>"
    echo "    Get MXE cross-compiling environment here: http://mxe.cc/"

    exit -1
fi

ZIPDIR=blockling-win32
ZIPFILE=blockling-win32.zip

# Delete the old zip file
rm -f $ZIPFILE

# Compile blockling.exe
MXE_PATH=$1
export PATH=$MXE_PATH/usr/bin:$PATH
make MXE_PATH=$MXE_PATH

# Create a directory to be zipped
mkdir $ZIPDIR

# Copy/move necessary files into the zip directory
cp -r ../../data ../../README.md ../../COPYING *.dll *.txt $ZIPDIR
mv blockling.exe $ZIPDIR

# Convert textfile linebreaks to Windows linebreaks and file extensions
perl -p -i -e 's/\n/\r\n/' $ZIPDIR/README.md
mv $ZIPDIR/README.md $ZIPDIR/readme.txt
perl -p -i -e 's/\n/\r\n/' $ZIPDIR/COPYING
mv $ZIPDIR/COPYING $ZIPDIR/license.txt

# Zip the directory
zip -r $ZIPFILE $ZIPDIR

# Delete the zip directory
rm -rf $ZIPDIR
