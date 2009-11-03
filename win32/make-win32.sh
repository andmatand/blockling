#CFLAGS="`/opt/mingw/usr/i386-mingw32msvc/bin/sdl-config --cflags`"
#LIBS="`/opt/mingw/usr/i386-mingw32msvc/bin/sdl-config --libs` -lSDL_mixer"

#/opt/mingw/usr/bin/i386-mingw32msvc-g++ -o blockman.exe ../main.cpp $CFLAGS $LIBS

/opt/mingw/usr/bin/i386-mingw32msvc-g++ -o blockman.exe ../main.cpp -D_GNU_SOURCE=1 -Dmain=SDL_main -I/opt/mingw/usr/i386-mingw32msvc/include/SDL -L/opt/mingw/usr/i386-mingw32msvc/lib -lmingw32 -lSDLmain SDL.dll SDL_mixer.dll -lwinmm -mwindows 

if [ -f blockman.exe ]; then
	/opt/mingw/usr/bin/i386-mingw32msvc-strip blockman.exe
	
	rm blockman-win32.zip
	zip -r blockman-win32 blockman.exe license.txt data/ readme-SDL.txt SDL.dll SDL_mixer.dll -x "*/.svn/*" -x "*~"
	rm blockman.exe
fi

# Upload the zip to billamonster.com/temp/
ftp -n hush.lunarbreeze.com <<END_SCRIPT
quote USER bman-build@billamonster.com
quote PASS bl0ckm4n4win32?get1inuXd00d
put blockman-win32.zip
quit
END_SCRIPT

