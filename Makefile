all: compile run clean

SRCS := $(wildcard *.cpp)
NAMES := $(patsubst %.cpp,%,${SRCS})

test:
	echo ${SRCS}
	echo ${NAMES}

compile: ${SRCS}
	./fltk/fltk-config --compile $<

run: ${NAMES}
	./$<

clean:
	rm -f ${NAMES}
