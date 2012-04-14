# Makefile for Rui Yang 764 Assignment 1 SMF Viewer

INCPATH = -I. -Imesh -Imathwork -Imarching_cubes/include

LIBPATH = -L/usr/local/lib -L/usr/X11R6/lib 

CC = g++

# executable file name
EXE = mcaq

CPPFLAGS = $(INCPATH) -O2 -g

LIBS_GLUT = -lGL -lGLU -lglut -lglui

GLUT =  mcaq.o

MESH =	mesh/vertex.o \
        mesh/edge.o \
    mesh/triangle.o \
    mesh/mesh.o
	
VEC =	mathwork/mathwork.o

MC = marching_cubes/mc_cube.o \
    marching_cubes/mc_cubestack.o \
    marching_cubes/marchingcubes.o

MC_TEST = marching_cubes/ut/mc_test

all: GLUT MC_UT

GLUT: $(GLUT) $(MESH) $(VEC) $(MC)
	$(CC) $? $(LIBPATH) $(LIBS_GLUT) -o $(EXE) -g

MC_UT: $(MESH) $(MC) $(VEC) marching_cubes/ut/mc_test.cpp ;\
    $(CC) -o $(MC_TEST) $? $(INCPATH) $(LIBPATH) $(LIBS_GLUT) -g

$(GLUT): mcaq.cpp ;\
    $(CC) -o $@ $< $(INCPATH) $(LIBPATH) $(LIBS_GLUT) -c -g

mesh/vertex.o: mesh/vertex.cpp mesh/*.h ;\
    $(CC) -o $@ $< $(INCPATH) $(LIBPATH) $(LIBS_GLUT) -c -g

mesh/edge.o: mesh/edge.cpp mesh/*.h ;\
    $(CC) -o $@ $< $(INCPATH) $(LIBPATH) $(LIBS_GLUT) -c -g

mesh/triangle.o: mesh/triangle.cpp mesh/*.h ;\
    $(CC) -o $@ $< $(INCPATH) $(LIBPATH) $(LIBS_GLUT) -c -g

mesh/mesh.o: mesh/mesh.cpp mesh/*.h ;\
    $(CC) -o $@ $< $(INCPATH) $(LIBPATH) $(LIBS_GLUT) -c -g


$(VEC): mathwork/mathwork.cpp mathwork/mathwork.h ;\
    $(CC) -o $@ $< $(INCPATH) $(LIBPATH) $(LIBS_GLUT) -c -g

marching_cubes/mc_cube.o: marching_cubes/src/mc_cube.cpp marching_cubes/include/*.h ;\
    $(CC) -o $@ $< $(INCPATH) $(LIBPATH) $(LIBS_GLUT) -c -g

marching_cubes/mc_cubestack.o: marching_cubes/src/mc_cubestack.cpp marching_cubes/include/*.h ;\
    $(CC) -o $@ $< $(INCPATH) $(LIBPATH) $(LIBS_GLUT) -c -g

marching_cubes/marchingcubes.o: marching_cubes/src/marchingcubes.cpp marching_cubes/include/*.h ;\
    $(CC) -o $@ $< $(INCPATH) $(LIBPATH) $(LIBS_GLUT) -c -g


clean: ;\
    rm -rf $(GLUT) $(MESH) $(VEC) $(MC) $(EXE)

.PHONY: clean
