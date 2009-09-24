#CFLAGS="`/opt/mingw/usr/i386-mingw32msvc/bin/sdl-config --cflags`"
#LIBS="`/opt/mingw/usr/i386-mingw32msvc/bin/sdl-config --libs` -lSDL_mixer"

#/opt/mingw/usr/bin/i386-mingw32msvc-g++ -o blockman.exe ../main.cpp $CFLAGS $LIBS

/opt/mingw/usr/bin/i386-mingw32msvc-g++ -o blockman.exe ../main.cpp -D_GNU_SOURCE=1 -Dmain=SDL_main -I/opt/mingw/usr/i386-mingw32msvc/include/SDL -L/opt/mingw/usr/i386-mingw32msvc/lib -lmingw32 -lSDLmain -lSDL_mixer -lSDL -lwinmm -mwindows

if [ -f blockman.exe ]; then
	/opt/mingw/usr/bin/strip blockman.exe
	
	rm blockman-win32.zip
	zip -r blockman-win32 blockman.exe COPYING data/ README-SDL.txt SDL.dll -x \*svn
	rm blockman.exe
fi
