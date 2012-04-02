# Makefile for Rui Yang 764 Assignment 1 SMF Viewer

INCPATH = -I. -Imesh -Imathwork

LIBPATH = -L/usr/local/lib -L/usr/X11R6/lib 

CC = g++

# executable file name
EXE = mcaq

CPPFLAGS = $(INCPATH) -O2

LIBS_GLUT = -lGL -lGLU -lglut -lglui

GLUT =  mcaq.o

MESH =	mesh/mesh.o \
	mesh/triangle.o \
	mesh/vertex.o \
        mesh/edge.o
	
VEC =	mathwork/mathwork.o

GLUT: $(GLUT) $(MESH) $(VEC)
	$(CC) $? $(LIBPATH) $(LIBS_GLUT) -o $(EXE)


