all: compile run

compile:
	./fltk/fltk-config --compile main.cpp

run: main
	./main
