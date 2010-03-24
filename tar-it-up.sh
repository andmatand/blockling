# This script creates a source archive for public release
filename=blockling-src-r$(svnversion | cut -d ":" -f 2 | egrep "[0-9]+" -o).tar.gz
tar -czf $filename --exclude=blockling*.tar.gz --exclude=blockling --exclude-vcs --exclude=.* --exclude tar-it-up.sh *

# Upload it to sourceforge
scp $filename andreanadelphon,blockling@frs.sourceforge.net:/home/frs/project/b/bl/blockling/
