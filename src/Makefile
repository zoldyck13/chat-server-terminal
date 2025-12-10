# ============================
# Makefile for FTXUI + SQLite
# ============================

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

FTXUI_FLAGS = $(shell pkg-config --cflags --libs ftxui)

LIBS = -lsqlite3

SRC = tester.cpp
OUT = output.out

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(FTXUI_FLAGS) $(LIBS)

clean:
	rm -f $(OUT)

.PHONY: all clean
