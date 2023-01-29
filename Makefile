level_editor: main.c
	gcc -o level_editor main.c level.c input.c -lm -I/usr/lib/i386-linux-gnu -lSDL2
