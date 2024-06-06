CC = gcc
SRC_DIR = src
BIN_DIR = bin
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3
FLAGS = $(DEBUG_FLAGS)
CFLAGS = -Wall -Wextra -std=c99 -pedantic $(FLAGS) -Wno-unused-function
LDFLAGS = -lm

interval:
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/interval.o $(SRC_DIR)/interval.c $(LDFLAGS)

stream_graph: interval
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/stream_graph.o $(SRC_DIR)/stream_graph.c $(LDFLAGS)
	ar rcs $(BIN_DIR)/stream_graph.a $(BIN_DIR)/interval.o $(BIN_DIR)/stream_graph.o

compile_lib:
	python3 setup.py build_ext --inplace