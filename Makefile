all: compile run clean

SRCS := $(wildcard *.cpp)
NAMES := $(patsubst %.cpp,%,${SRCS})

test:
	echo ${SRCS}
	echo ${NAMES}

compile:
	./fltk/fltk-config --compile main.cpp

run: main
	./main.out

clean:
	rm -f ${NAMES}
