CC = g++
CFLAGS = -Wall
LIBRARIES = -lGL -lGLU -lglut -lpng -lm
OBJECTS = $(patsubst %.cpp, %.o, $(wildcard src/*.cpp))

all: $(OBJECTS)
	$(CC) -o snake3D $(LIBRARIES) $(OBJECTS)

%.o: %.cpp
	$(CC) -c -o $@ $(CFLAGS) $<

clean:
	rm -f ./openGLtest
	rm -f src/*.o
	rm -f *~
