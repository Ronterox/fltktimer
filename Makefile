CXX=$(shell fltk-config --cxx)
CFLAGS=-Wall -Werror -Wextra -Wpedantic
DEBUG=-g
# DEBUG=-O3

LIGHT=-bg "\#F5F5F7" -bg2 "\#E8E8ED" -fg "\#333333" -scheme "oxy" -title "LightApp" -tooltips
DARK=-bg "\#1E1E1E" -bg2 "\#252526" -fg "\#D4D4D4" -scheme "gleam" -title "ModernApp" -tooltips
DESIGN=$(LIGHT)
TASKFILE=tasks

CXXFLAGS = $(shell fltk-config --use-gl --use-images --cxxflags) -I.
LDFLAGS  = $(shell fltk-config --use-gl --use-images --ldflags)
LDSTATIC = $(shell fltk-config --use-gl --use-images --ldstaticflags)
LINK     = $(CXX)

TARGET = tasks.out
OBJS = main.o
SRCS = src/main.cpp

all: run clean

run: $(TARGET)
	./$(TARGET) $(DESIGN) $(TASKFILE)

.SUFFIXES: .cxx .o
$(OBJS): $(SRCS)
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(DEBUG) -c $<

$(TARGET): $(OBJS)
	$(LINK) -o $(TARGET) $(OBJS) $(LDFLAGS)

gdb: $(TARGET)
	gdb ./$(TARGET)

clean: $(TARGET) $(OBJS)
	rm -f $(OBJS) $(TARGET) 2> /dev/null
