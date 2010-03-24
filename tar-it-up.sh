# This script creates a source archive for public release

# Delete previous tarballs
rm blockling-src*.tar.gz

# Construct the filename from current svn revision number
filename=blockling-src-r$(svnversion | cut -d ":" -f 2 | egrep "[0-9]+" -o).tar.gz

# Make the current tarball
tar -czf $filename --exclude=blockling*.tar.gz --exclude=blockling --exclude-vcs --exclude=.* --exclude=win32 --exclude tar-it-up.sh *

# Upload it to sourceforge
scp $filename andreanadelphon,blockling@frs.sourceforge.net:/home/frs/project/b/bl/blockling/
