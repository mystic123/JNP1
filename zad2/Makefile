CPP=g++
RELEASE_FLAGS=-c -O2 -Wall -std=c++0x
DEBUG_FLAGS=-c -g -D DEBUG_MODE=1 -Wall -std=c++0x
FLAGS=
OBJS= network growingnet
debuglevel = 0

ifeq ($(debuglevel), 0)
	FLAGS = $(RELEASE_FLAGS)
else
	FLAGS = $(DEBUG_FLAGS)
endif

all: $(OBJS)

network: network.cc network.h
	$(CPP) $(FLAGS) network.cc -o network.o

growingnet: growingnet.cc growingnet.h
	$(CPP) $(FLAGS) growingnet.cc -o growingnet.o

clean:
	rm -f *.o
