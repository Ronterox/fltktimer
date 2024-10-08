CXX=$(shell fltk-config --cxx)
CFLAGS=-Wall -Werror -Wextra -Wpedantic
DEBUG=-g

CXXFLAGS = $(shell fltk-config --use-gl --use-images --cxxflags) -I.
LDFLAGS  = $(shell fltk-config --use-gl --use-images --ldflags)
LDSTATIC = $(shell fltk-config --use-gl --use-images --ldstaticflags)
LINK     = $(CXX)

TARGET = main
OBJS = main.o
SRCS = src/main.cpp

all: run clean

run: $(TARGET)
	./$(TARGET)

.SUFFIXES: .cxx .o
$(OBJS): $(SRCS)
	$(CXX) $(CXXFLAGS) $(CFLAGS) $(DEBUG) -c $<

$(TARGET): $(OBJS)
	$(LINK) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean: $(TARGET) $(OBJS)
	rm -f *.o  2> /dev/null
	rm -f $(TARGET) 2> /dev/null
