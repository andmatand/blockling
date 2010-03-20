# This script creates a source archive for public release
# For a win32 binary release, see the script in win32/
tar -czf ../blockling.tar.gz --exclude=win32 --exclude=.svn --exclude tar-it-up.sh *
