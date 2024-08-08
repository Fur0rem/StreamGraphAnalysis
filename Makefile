CC = gcc
SRC_DIR = src
BIN_DIR = bin
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3
FLAGS = $(DEBUG_FLAGS)
# CFLAGS = -Wall -Wextra $(FLAGS) -Wno-unused-function -std=c2x
CFLAGS = $(FLAGS) -std=c2x
LDFLAGS = -lm

# Ensure the binary directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

utils: $(SRC_DIR)/utils.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/utils.o

defaults: $(SRC_DIR)/defaults.c utils | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/defaults.c -o $(BIN_DIR)/defaults.o
	ar rcs $(BIN_DIR)/defaults.a $(BIN_DIR)/defaults.o $(BIN_DIR)/utils.o

units: $(SRC_DIR)/units.c defaults | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/units.c -o $(BIN_DIR)/units.o
	ar rcs $(BIN_DIR)/units.a $(BIN_DIR)/units.o $(BIN_DIR)/defaults.a

bit_array: $(SRC_DIR)/bit_array.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/bit_array.o

interval: $(SRC_DIR)/interval.c defaults utils | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/interval.c -o $(BIN_DIR)/interval.o
	ar rcs $(BIN_DIR)/interval.a $(BIN_DIR)/interval.o $(BIN_DIR)/defaults.o $(BIN_DIR)/utils.o

events_table: $(SRC_DIR)/stream_graph/events_table.c defaults | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/events_table.o
	ar rcs $(BIN_DIR)/events_table.a $(BIN_DIR)/events_table.o $(BIN_DIR)/defaults.a

key_moments_table: $(SRC_DIR)/stream_graph/key_moments_table.c defaults | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/key_moments_table.o
	ar rcs $(BIN_DIR)/key_moments_table.a $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/defaults.a

links_set: $(SRC_DIR)/stream_graph/links_set.c defaults | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/links_set.o
	ar rcs $(BIN_DIR)/links_set.a $(BIN_DIR)/links_set.o $(BIN_DIR)/defaults.a

nodes_set: $(SRC_DIR)/stream_graph/nodes_set.c defaults | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/nodes_set.o
	ar rcs $(BIN_DIR)/nodes_set.a $(BIN_DIR)/nodes_set.o $(BIN_DIR)/defaults.a

stream: $(SRC_DIR)/stream.c events_table key_moments_table links_set nodes_set defaults interval utils bit_array | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/stream.c -o $(BIN_DIR)/stream.o
	ar rcs $(BIN_DIR)/stream.a $(BIN_DIR)/stream.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/defaults.o $(BIN_DIR)/interval.o $(BIN_DIR)/utils.o $(BIN_DIR)/bit_array.o

induced_graph: $(SRC_DIR)/induced_graph.c interval stream defaults | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/induced_graph.c -o $(BIN_DIR)/induced_graph.o
	ar rcs $(BIN_DIR)/induced_graph.a $(BIN_DIR)/interval.a $(BIN_DIR)/stream.a $(BIN_DIR)/induced_graph.o $(BIN_DIR)/defaults.a

# full_stream_graph: $(SRC_DIR)/stream/full_stream_graph.c interval stream defaults | $(BIN_DIR)
# 	$(CC) $(CFLAGS) -c $(SRC_DIR)/stream/full_stream_graph.c -o $(BIN_DIR)/full_stream_graph.o
# 	ar rcs $(BIN_DIR)/full_stream_graph.a $(BIN_DIR)/interval.a $(BIN_DIR)/stream.a $(BIN_DIR)/full_stream_graph.o $(BIN_DIR)/defaults.a

full_stream_graph: $(SRC_DIR)/stream/full_stream_graph.c interval stream defaults induced_graph events_table key_moments_table links_set nodes_set utils bit_array | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/stream/full_stream_graph.c -o $(BIN_DIR)/full_stream_graph.o
	ar rcs $(BIN_DIR)/full_stream_graph.a bin/full_stream_graph.o bin/events_table.o bin/stream.o bin/key_moments_table.o bin/interval.o bin/utils.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/defaults.o bin/induced_graph.o bin/events_table.o bin/stream.o

link_stream: $(SRC_DIR)/stream/link_stream.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/link_stream.o

chunk_stream: $(SRC_DIR)/stream/chunk_stream.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/chunk_stream.o

chunk_stream_small: $(SRC_DIR)/stream/chunk_stream_small.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/chunk_stream_small.o

iterators: $(SRC_DIR)/iterators.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/iterators.o

metrics: $(SRC_DIR)/metrics.c stream interval defaults utils full_stream_graph link_stream chunk_stream chunk_stream_small bit_array nodes_set links_set events_table key_moments_table induced_graph interval iterators units | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/metrics.c -o $(BIN_DIR)/metrics.o
	ar rcs $(BIN_DIR)/metrics.a $(BIN_DIR)/metrics.o $(BIN_DIR)/stream.o $(BIN_DIR)/interval.o $(BIN_DIR)/defaults.o $(BIN_DIR)/utils.o $(BIN_DIR)/full_stream_graph.o $(BIN_DIR)/link_stream.o $(BIN_DIR)/chunk_stream.o $(BIN_DIR)/chunk_stream_small.o $(BIN_DIR)/bit_array.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/links_set.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/induced_graph.o $(BIN_DIR)/interval.o $(BIN_DIR)/iterators.o $(BIN_DIR)/defaults.o $(BIN_DIR)/utils.o $(BIN_DIR)/units.o

arena:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/arena.c -o $(BIN_DIR)/arena.o
	ar rcs $(BIN_DIR)/arena.a $(BIN_DIR)/arena.o

walks: $(SRC_DIR)/walks.c iterators utils units defaults interval arena full_stream_graph chunk_stream_small chunk_stream link_stream stream bit_array nodes_set links_set events_table key_moments_table induced_graph metrics | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/walks.c -o $(BIN_DIR)/walks.o
	ar rcs $(BIN_DIR)/walks.a $(BIN_DIR)/walks.o $(BIN_DIR)/iterators.o $(BIN_DIR)/utils.o $(BIN_DIR)/units.o $(BIN_DIR)/defaults.o $(BIN_DIR)/interval.o $(BIN_DIR)/arena.o $(BIN_DIR)/full_stream_graph.o $(BIN_DIR)/chunk_stream_small.o $(BIN_DIR)/chunk_stream.o $(BIN_DIR)/link_stream.o $(BIN_DIR)/stream.o $(BIN_DIR)/bit_array.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/links_set.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/induced_graph.o $(BIN_DIR)/metrics.o 

cliques: $(SRC_DIR)/cliques.c iterators utils units defaults interval arena full_stream_graph chunk_stream_small chunk_stream link_stream stream bit_array nodes_set links_set events_table key_moments_table induced_graph metrics walks | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/cliques.c -o $(BIN_DIR)/cliques.o
	ar rcs $(BIN_DIR)/cliques.a $(BIN_DIR)/cliques.o $(BIN_DIR)/iterators.o $(BIN_DIR)/utils.o $(BIN_DIR)/units.o $(BIN_DIR)/defaults.o $(BIN_DIR)/interval.o $(BIN_DIR)/arena.o $(BIN_DIR)/full_stream_graph.o $(BIN_DIR)/chunk_stream_small.o $(BIN_DIR)/chunk_stream.o $(BIN_DIR)/link_stream.o $(BIN_DIR)/stream.o $(BIN_DIR)/bit_array.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/links_set.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/induced_graph.o $(BIN_DIR)/metrics.o $(BIN_DIR)/walks.o
	
clean:
	rm -f $(BIN_DIR)/*