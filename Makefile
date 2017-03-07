all:
	gcc -Wall -Wextra -Wunused -g sequencefile.c statistics.c suffixtries.c bitmap.c graphics.c 3dgraphics.c entropicprofiles.c -lm -o ep
