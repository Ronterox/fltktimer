all: compile run clean

SRCS := $(wildcard *.cpp)

test:
	echo ${SRCS}

compile:
	./fltk/fltk-config --compile main.cpp

run: main
	./main.out

clean:
	rm -f main.out
