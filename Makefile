
INCLUDE = -I/usr/include -I/usr/X11R6/include -I. -I./sgl 
LIBDIR = -L. -L/usr/X11R6/lib -L/usr/lib

COMPILERFLAGS = -Wall -O2 -DUSE_GUI=1
#-g 
CC = g++
CFLAGS = $(COMPILERFLAGS) $(INCLUDE) 

LIBRARIES = -lglut -lGL -lGLU  #-lXext -lX11 -lm
#LIBRARIES = -lm
OBJS = testapp.o sgl/sgl.o sgl/Context.o sgl/SglFacade.o nffread.o timer.o hdrloader.o
OBJS2 = raytracer.o sgl/sgl.o nffread.o timer.o hdrloader.o

#-lXmu -lXi 
default:
	make testapp

testapp : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(LIBDIR) $(OBJS) $(LIBRARIES)  


.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp -o $*.o

clean:
	rm -f testapp $(OBJS)

