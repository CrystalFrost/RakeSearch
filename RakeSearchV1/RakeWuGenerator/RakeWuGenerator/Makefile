
BOINC_DIR = /home/hoarfrost/Projects/BOINC
BOINC_API_DIR = $(BOINC_DIR)/api
BOINC_LIB_DIR = $(BOINC_DIR)/lib

CXXFLAGS += -O2 -static \
    -I$(BOINC_DIR) \
    -I$(BOINC_LIB_DIR) \
    -I$(BOINC_API_DIR) \
    -L$(BOINC_API_DIR) \
    -L$(BOINC_LIB_DIR) \

PROGS = main 

all: $(PROGS)

clean: distclean

distclean:
	rm -f $(PROGS) *.o RakeWuGenerator

main: main.o Square.o Generator.o 
	g++ $(CXXFLAGS) $(LDFLAGS) -o RakeWuGenerator main.o Square.o Generator.o 

Square.o: Square.cpp
	g++ -c Square.cpp

Generator.o: Generator.cpp
	g++ -c Generator.cpp
