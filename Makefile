CC = gcc
SRC_DIR = src
BIN_DIR = bin
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3
FLAGS = $(DEBUG_FLAGS)
CFLAGS = -Wall -Wextra $(FLAGS) -Wno-unused-function -std=c2x
LDFLAGS = -lm

iterators:
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/iterators.o $(SRC_DIR)/iterators.c $(LDFLAGS)

bit_array:
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/bit_array.o $(SRC_DIR)/bit_array.c $(LDFLAGS)

interval:
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/interval.o $(SRC_DIR)/interval.c $(LDFLAGS)

events_table: interval bit_array
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/events_table.o $(SRC_DIR)/stream_graph/events_table.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/events_table.a $(BIN_DIR)/events_table.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o

key_moments_table: interval bit_array
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/key_moments_table.o $(SRC_DIR)/stream_graph/key_moments_table.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/key_moments_table.a $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o

links_set: interval
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/links_set.o $(SRC_DIR)/stream_graph/links_set.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/links_set.a $(BIN_DIR)/links_set.o $(BIN_DIR)/interval.o

nodes_set: interval
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/nodes_set.o $(SRC_DIR)/stream_graph/nodes_set.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/nodes_set.a $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o

# TODO: Make better dependencies, same for the metrics target
stream_graph: events_table key_moments_table links_set nodes_set interval bit_array
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/stream_graph.o $(SRC_DIR)/stream_graph.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/stream_graph.a $(BIN_DIR)/stream_graph.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o

induced_graph: stream_graph
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/induced_graph.o $(SRC_DIR)/induced_graph.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/induced_graph.a $(BIN_DIR)/induced_graph.o $(BIN_DIR)/stream_graph.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o
	
full_stream_graph: stream_graph induced_graph
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/full_stream_graph.o $(SRC_DIR)/stream/full_stream_graph.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/full_stream_graph.a $(BIN_DIR)/full_stream_graph.o $(BIN_DIR)/induced_graph.o $(BIN_DIR)/stream_graph.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o

link_stream: stream_graph
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/link_stream.o $(SRC_DIR)/stream/link_stream.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/link_stream.a $(BIN_DIR)/link_stream.o $(BIN_DIR)/stream_graph.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o

chunk_stream: stream_graph
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/chunk_stream.o $(SRC_DIR)/stream/chunk_stream.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/chunk_stream.a $(BIN_DIR)/chunk_stream.o $(BIN_DIR)/stream_graph.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o

metrics: full_stream_graph link_stream induced_graph iterators chunk_stream
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/metrics.o $(SRC_DIR)/metrics.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/metrics.a $(BIN_DIR)/metrics.o $(BIN_DIR)/full_stream_graph.o $(BIN_DIR)/link_stream.o $(BIN_DIR)/stream_graph.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o $(BIN_DIR)/induced_graph.o $(BIN_DIR)/iterators.o $(BIN_DIR)/chunk_stream.o