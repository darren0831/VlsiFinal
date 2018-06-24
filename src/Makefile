# Makefile


CC := g++

CXXFLAGS := -std=c++17 -O3 -Wall -Wextra -Wno-unused-result

LDFLAGS := -lm -pthread

DEFFLAGS := -DVLSI_FINAL_PROJECT_DEBUG_FLAG

BIN := route


.PHONY: all clean


all: main.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(DEFFLAGS) -o $(BIN) $<

debug: main.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(DEFFLAGS) -o $(BIN) $<

release: main.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) -o $(BIN) $<

clean:
	-rm -rf *.o $(BIN)
