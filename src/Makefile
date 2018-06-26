# Makefile


CC := g++

CXXFLAGS := -O3 -std=c++17 -Wall -Wextra -Wno-unused-result

LDFLAGS := -pthread

DEBUGFLAGS := -DVLSI_FINAL_PROJECT_DEBUG_FLAG

RELEASEFLAGS := -DVLSI_FINAL_PROJECT_RELEASE_FLAG

BIN := route


.PHONY: all clean


all: main.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(DEBUGFLAGS) -o $(BIN) $<

debug: main.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(DEBUGFLAGS) -o $(BIN) $<

release: main.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(RELEASEFLAGS) -o $(BIN) $<

clean:
	-rm -rf *.o $(BIN)
