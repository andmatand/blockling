# This script creates a source archive for public release
tar -czf blockling.tar.gz --exclude=blockling.tar.gz --exclude=win32 --exclude=.svn --exclude tar-it-up.sh *
