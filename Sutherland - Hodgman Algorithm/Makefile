CC = gcc
CFLAGS = -g -Wall

ifeq "$(OS)" "Windows_NT"
# Windows
EXECS = clipping_polygons.exe
LIBS = -lfreeglut -lopengl32 -lglu32
IFLAGS = -I".\freeglut\include"
LFLAGS =  -L".\freeglut\lib"
RM = del

all: $(EXECS)
clipping_polygons.exe: clipping_polygons.o
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(LIBS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< $(IFLAGS)

clean:
	if exist *.exe $(RM) *.exe
	if exist *.o   $(RM) *.o
else
# Linux
TARGETS = clipping_polygons
LIBS = -lm -lGL -lglut -lGLU
RM = rm -f

all: $(TARGETS)
%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
%.o: %.c    #Suppress default rule
	$(CC) $(CFLAGS) -c $@ $<

clean:
	$(RM) $(TARGETS) *.o
endif
