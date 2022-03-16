CC = g++
CFLAGS = -g -Wall

ifeq "$(OS)" "Windows_NT"
# Windows
EXECS = views.exe visiting3Dhouse.exe
LIBS = -lfreeglut -lopengl32 -lglu32
IFLAGS = -I"..\freeglut\include"
LFLAGS =  -L"..\freeglut\lib"
RM = del

all: $(EXECS)
views.exe: views.o
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(LIBS)
visiting3Dhouse.exe: visiting3Dhouse.o
	$(CC) $(CFLAGS) -o $@ $< $(LFLAGS) $(LIBS)
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< $(IFLAGS)

clean:
	if exist *.exe $(RM) *.exe
	if exist *.o   $(RM) *.o
else
# Linux
TARGETS = views visiting3Dhouse
LIBS = -lm -lGL -lglut -lGLU
RM = rm -f

all: $(TARGETS)
%: %.cpp
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)
%.o: %.cpp    #Suppress default rule
	$(CC) $(CFLAGS) -c $@ $<

clean:
	$(RM) $(TARGETS) *.o
endif
