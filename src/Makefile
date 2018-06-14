# Makefile


CC := g++

CXXFLAGS := -std=c++14 -O3 -Wno-unused-result

LDFLAGS :=

BIN := route


.PHONY: all clean


all: main.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) -o $(BIN) $<

clean:
	-rm -rf *.o $(BIN)
