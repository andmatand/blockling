i586-mingw32msvc-g++ -o blockman.exe ../main.cpp -lmingw32 -lSDLmain -lSDL -I/usr/i586-mingw32msvc/include/SDL

if [ -f blockman.exe ]; then
	/usr/i586-mingw32msvc/bin/strip blockman.exe
	
	rm blockman-win32.zip
	zip -r blockman-win32 blockman.exe data/ README-SDL.txt SDL.dll -x \*svn
	rm blockman.exe
fi
