BOINC_DIR = /home/hoarfrost/Projects/BOINC
BOINC_API_DIR = $(BOINC_DIR)/api
BOINC_LIB_DIR = $(BOINC_DIR)/lib

COMPILER = g++

CXXFLAGS += -O2 -static \
    -I$(BOINC_DIR) \
    -I$(BOINC_LIB_DIR) \
    -I$(BOINC_API_DIR) \
    -L$(BOINC_API_DIR) \
    -L$(BOINC_LIB_DIR) \

PROGRAM = rakesearch

all: $(PROGRAM)

clean:
	rm -f $(PROGRAM) *.o

$(PROGRAM): main.o Square.o Generator.o MovePairSearch.o $(BOINC_LIB_DIR)/libboinc.a $(BOINC_API_DIR)/libboinc_api.a
	$(COMPILER) $(CXXFLAGS) -o $(PROGRAM) main.o Square.o Generator.o MovePairSearch.o -pthread -lboinc_api -lboinc

Square.o: Square.cpp
	$(COMPILER) $(CXXFLAGS) -c Square.cpp

Generator.o: Generator.cpp
	$(COMPILER) $(CXXFLAGS) -c Generator.cpp
	
MovePairSearch.o: MovePairSearch.cpp
	$(COMPILER) $(CXXFLAGS) -c MovePairSearch.cpp 

main.o: main.cpp
	$(COMPILER) $(CXXFLAGS) -c main.cpp
