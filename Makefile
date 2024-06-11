CC = gcc
SRC_DIR = src
BIN_DIR = bin
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3
FLAGS = $(DEBUG_FLAGS)
CFLAGS = -Wall -Wextra -pedantic $(FLAGS) -Wno-unused-function
LDFLAGS = -lm

bit_array:
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/bit_array.o $(SRC_DIR)/bit_array.c $(LDFLAGS)

interval:
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/interval.o $(SRC_DIR)/interval.c $(LDFLAGS)

events_table: interval bit_array
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/events_table.o $(SRC_DIR)/stream_graph/events_table.c $(LDFLAGS)
	ar rcs $(BIN_DIR)/events_table.a $(BIN_DIR)/events_table.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o

key_moments_table: interval bit_array
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/key_moments_table.o $(SRC_DIR)/stream_graph/key_moments_table.c $(LDFLAGS)
	ar rcs $(BIN_DIR)/key_moments_table.a $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o

links_set: interval
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/links_set.o $(SRC_DIR)/stream_graph/links_set.c $(LDFLAGS)
	ar rcs $(BIN_DIR)/links_set.a $(BIN_DIR)/links_set.o $(BIN_DIR)/interval.o

nodes_set: interval
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/nodes_set.o $(SRC_DIR)/stream_graph/nodes_set.c $(LDFLAGS)
	ar rcs $(BIN_DIR)/nodes_set.a $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o

# TODO: Make better dependencies, same for the measures target
stream_graph: events_table key_moments_table links_set nodes_set interval bit_array
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/stream_graph.o $(SRC_DIR)/stream_graph.c $(LDFLAGS)
	ar rcs $(BIN_DIR)/stream_graph.a $(BIN_DIR)/stream_graph.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o

measures: stream_graph
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/measures.o $(SRC_DIR)/measures.c $(LDFLAGS)
	ar rcs $(BIN_DIR)/measures.a $(BIN_DIR)/stream_graph.o $(BIN_DIR)/measures.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o

compile_lib:
	python3 setup.py build_ext --inplace