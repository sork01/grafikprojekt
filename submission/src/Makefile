.PHONY: all clean

CC=gcc
CFLAGS=-O3 -Wall -I/usr/include/glm `pkg-config --cflags gtk+-3.0 sdl2 SDL2_image lua-5.3-c++`
LDFLAGS=-pthread -lstdc++ -lm `pkg-config --libs gtk+-3.0 sdl2 SDL2_image lua-5.3-c++`
HEADERS=tribox3.h camera.hpp filedialog.hpp gtkfiledialog.hpp gui.hpp intersection.hpp light.hpp material.hpp mesh.hpp object.hpp objreader.hpp pixelsurface.hpp raytracingrenderer.hpp renderer.hpp scene.hpp sdlgui.hpp sdltexture.hpp sphere.hpp stlreader.hpp texture.hpp usbscancodes.hpp wireframeelement.hpp wireframerenderer.hpp
SOURCES=tribox3.c camera.cpp gtkfiledialog.cpp light.cpp main.cpp material.cpp mesh.cpp objreader.cpp raytracingrenderer.cpp scene.cpp sdlgui.cpp sdltexture.cpp sphere.cpp stlreader.cpp texture.cpp wireframeelement.cpp wireframerenderer.cpp
OBJECTS=tribox3.o camera.o gtkfiledialog.o light.o main.o material.o mesh.o objreader.o raytracingrenderer.o scene.o sdlgui.o sdltexture.o sphere.o stlreader.o wireframeelement.o wireframerenderer.o
EXECUTABLE=main

all: $(OBJECTS) $(EXECUTABLE)

clean:
	rm -f *.o
	rm -f main

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< $(LDFLAGS) -o $@
