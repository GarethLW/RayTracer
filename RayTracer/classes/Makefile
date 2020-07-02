# For OS X

CC=clang++
CFLAGS=-Wall -std=c++11 -g -DDEBUG

SRC=./src
OUT=./build
FLTK=./fltk
GLEW=./glew

LIBDIRS=-L$(FLTK)/lib -L$(GLEW)/lib
LIBS=-lfltk -lfltk_forms -lfltk_gl -lfltk_images -lGLEW
INCLUDES=-I$(FLTK)/include -I$(GLEW)/include
FRAMEWORKS=-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

target = $(OUT)/a1
sources = $(wildcard $(SRC)/*.cpp $(SRC)/*.c $(SRC)/*.C)

$(target):	$(sources) $(wildcard $(SRC)/*.hpp $(SRC)/*.h $(SRC)/*.H)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBDIRS) $(LIBS) $(FRAMEWORKS) $(sources) -o $@

clean:
	rm -f $(target)
