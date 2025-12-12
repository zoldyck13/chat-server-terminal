# ============================
# Makefile for Server & Client
# using FTXUI + SQLite3
# ============================

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

FTXUI_FLAGS = $(shell pkg-config --cflags --libs ftxui)
LIBS = -lsqlite3

# ----------------------------
# Source files per target
# ----------------------------
SERVER_SRC = tester.cpp server_h.cpp Dbserver.cpp
CLIENT_SRC = client_ftxui.cpp ClientSocket.cpp

SERVER_OUT = server.out
CLIENT_OUT = clientui.out

# ----------------------------
# Build both
# ----------------------------
all: server client

# ----------------------------
# Build server
# ----------------------------
server: $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(SERVER_SRC) -o $(SERVER_OUT) $(FTXUI_FLAGS) $(LIBS)
	@echo "✔ Server built: ./$(SERVER_OUT)"

# ----------------------------
# Build client
# ----------------------------
client: $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRC) -o $(CLIENT_OUT) $(FTXUI_FLAGS) $(LIBS)
	@echo "✔ Client built: ./$(CLIENT_OUT)"

# ----------------------------
# Clean binaries
# ----------------------------
clean:
	rm -f $(SERVER_OUT) $(CLIENT_OUT)

.PHONY: all server client clean
