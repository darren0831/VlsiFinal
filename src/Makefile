# Makefile


CC := g++

CXXFLAGS := -std=c++14 -O3 -Wall -Wextra -Wno-unused-result

LDFLAGS := -lm

BIN := route


.PHONY: all clean


all: main.cpp
	$(CC) $(CXXFLAGS) $(LDFLAGS) -o $(BIN) $<

clean:
	-rm -rf *.o $(BIN)
