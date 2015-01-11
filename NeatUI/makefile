CC = gcc # Compiler to use
OPTIONS = -I. -O2 -g -Wall -std=c99 -pedantic-errors

all: main.o env.o ui.o map.o common.o
	${CC} ${OPTIONS} obj/main.o obj/env.o obj/ui.o obj/map.o obj/common.o -o target_bin

main.o: Environment/env.h UI/ui.h Map/map.h
	${CC} ${OPTIONS} -c main.c -o obj/main.o

env.o: common.h
	${CC} ${OPTIONS} -c Environment/env.c -o obj/env.o

ui.o: io.o buffer.o renderer.o Environment/env.h common.h
	${CC} ${OPTIONS} -c UI/ui.c  -o obj/ui_raw.o
	ld -r obj/io.o obj/buffer.o obj/renderer.o obj/ui_raw.o -o obj/ui.o
	rm obj/ui_raw.o

io.o: common.h
	${CC} ${OPTIONS} -c UI/io.c -o obj/io.o

buffer.o: common.h  UI/io.h
	${CC} ${OPTIONS} -c UI/buffer.c -o obj/buffer.o

renderer.o: common.h UI/buffer.h
	${CC} ${OPTIONS} -c UI/renderer.c -o obj/renderer.o

map.o: common.h Environment/env.h
	${CC} ${OPTIONS} -c Map/map.c -o obj/map.o

common.o: common.h
	${CC} ${OPTIONS} -c common.c -o obj/common.o

clean:
	rm -f obj/*.o
	rm -f target_bin
