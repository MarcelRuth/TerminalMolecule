# -*- Makefile -*-
all: HaloAnimation.x
HaloAnimation.x: main_linux.cpp
	g++ main_linux.cpp -o HaloAnimation.x
# clean up
clean:
	rm -f *.x *.o

# for mac 
# g++ -std=c++11 -Wall main.cpp 
