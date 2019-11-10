BOINC_DIR = /home/pi/pi_projects/BOINC/boinc
BOINC_API_DIR = $(BOINC_DIR)/api
BOINC_LIB_DIR = $(BOINC_DIR)/lib

COMPILER = g++

CXXFLAGS += -O3 -mcpu=cortex-a53 -mtune=cortex-a53 -static -std=c++11 \
    -I$(BOINC_DIR) \
    -I$(BOINC_LIB_DIR) \
    -I$(BOINC_API_DIR) \
    -L$(BOINC_API_DIR) \
    -L$(BOINC_LIB_DIR) \

PROGRAM = rakesearch

all: $(PROGRAM)

clean:
	rm -f $(PROGRAM) *.o

$(PROGRAM): main.o Square.o RakeSearch.o $(BOINC_LIB_DIR)/libboinc.a $(BOINC_API_DIR)/libboinc_api.a
	$(COMPILER) $(CXXFLAGS) -o $(PROGRAM) main.o Square.o RakeSearch.o -pthread -lboinc_api -lboinc

Square.o: Square.cpp
	$(COMPILER) $(CXXFLAGS) -c Square.cpp

RakeSearch.o: RakeSearch.cpp
	$(COMPILER) $(CXXFLAGS) -c RakeSearch.cpp

main.o: main.cpp
	$(COMPILER) $(CXXFLAGS) -c main.cpp
