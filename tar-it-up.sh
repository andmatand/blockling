# This script creates a source archive for public release
tar -czf blockling-src-r$(svnversion | cut -d ":" -f 2 | egrep "[0-9]+" -o).tar.gz --exclude=blockling.tar.gz --exclude=blockling --exclude=win32 --exclude=.svn --exclude tar-it-up.sh *
