CXXFLAGS += -O2 -static -std=c++11

PROGRAM = Square

all: $(PROGRAM)

clean:
	rm -f $(PROGRAM) *.o Square

$(PROGRAM): main.o Square.o DLX_DLS.o
	g++ $(CXXFLAGS) -o $(PROGRAM) main.o Square.o DLX_DLS.o

Square.o: Square.cpp
	g++ $(CXXFLAGS) -c Square.cpp

DLX_DLS.o: DLX_DLS.cpp
	g++ $(CXXFLAGS) -c DLX_DLS.cpp

main.o: main.cpp
	g++ $(CXXFLAGS) -c main.cpp