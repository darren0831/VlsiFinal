# Makefile


CC := g++

CXXFLAGS := -O3 -std=c++17 -Wall -Wextra -Wno-unused-result

LDFLAGS := -pthread

DEBUGFLAGS := -DVLSI_FINAL_PROJECT_DEBUG_FLAG

RELEASEFLAGS := -DVLSI_FINAL_PROJECT_RELEASE_FLAG

BIN := route

VERBIN := verifier


.PHONY: all debug release verifier clean


all: main.cpp verifier.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(DEBUGFLAGS) -o $(BIN) $<
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(RELEASEFLAGS) -o $(VERBIN) $<

debug: main.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(DEBUGFLAGS) -o $(BIN) $<

release: main.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(RELEASEFLAGS) -o $(BIN) $<

verifier: verifier.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(RELEASEFLAGS) -o $(VERBIN) $<

clean:
	-rm -rf *.o $(BIN) $(VERBIN)
