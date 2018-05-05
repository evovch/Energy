FLAGS=-Wall -Wextra -std=c++11
ROOTFLAGS=-I`root-config --incdir` -L`root-config --libdir` -lCore -lRIO -lTree

.PHONY: clean all

all: server.exe client.exe client_real.exe

server.exe: server.cpp
	g++ $^ -o $@ $(FLAGS)

client.exe: client.cpp
	g++ $^ -o $@ $(FLAGS) $(ROOTFLAGS)

client_real.exe: client_real.cpp
	g++ $^ -o $@ $(FLAGS) $(ROOTFLAGS)

clean:
	rm -rf *.exe *.root
