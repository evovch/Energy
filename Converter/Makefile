INCLUDES = -I/home/evovch/soft/root_install/include/

LIBS = `root-config --libs`

FLAGS = -std=c++11 -Wall -Wextra 
#-Wpedantic

.PHONY: all clean

all:
	g++ Converter.cpp -o converter.exe $(INCLUDES) $(LIBS) $(FLAGS)
	g++ ReadRootFile.cpp -o readRootFile.exe $(INCLUDES) $(LIBS) $(FLAGS)

clean:
	rm -fv *.exe
