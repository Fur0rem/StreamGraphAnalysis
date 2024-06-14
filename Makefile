CC = gcc
SRC_DIR = src
BIN_DIR = bin
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3
FLAGS = $(DEBUG_FLAGS)
CFLAGS = -Wall -Wextra -pedantic $(FLAGS) -Wno-unused-function
LDFLAGS = -lm

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
	
metrics_node : stream_graph
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/metrics_node.o $(SRC_DIR)/metrics/node.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/metrics_node.a $(BIN_DIR)/stream_graph.o $(BIN_DIR)/metrics_node.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o

metrics_link : stream_graph
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/metrics_link.o $(SRC_DIR)/metrics/link.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/metrics_link.a $(BIN_DIR)/stream_graph.o $(BIN_DIR)/metrics_link.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o

metrics: stream_graph metrics_node metrics_link
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/metrics.o $(SRC_DIR)/metrics.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/metrics.a $(BIN_DIR)/stream_graph.o $(BIN_DIR)/metrics.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/metrics_node.o $(BIN_DIR)/metrics_link.o

full_stream_graph: stream_graph
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/full_stream_graph.o $(SRC_DIR)/full_stream_graph.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/full_stream_graph.a $(BIN_DIR)/full_stream_graph.o $(BIN_DIR)/stream_graph.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o

link_stream: stream_graph
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/link_stream.o $(SRC_DIR)/link_stream.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/link_stream.a $(BIN_DIR)/link_stream.o $(BIN_DIR)/stream_graph.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o

metrics_generic: full_stream_graph link_stream
	@ $(CC) $(CFLAGS) -c -o $(BIN_DIR)/metrics_generic.o $(SRC_DIR)/metrics_generic.c $(LDFLAGS)
	@ ar rc $(BIN_DIR)/metrics_generic.a $(BIN_DIR)/metrics_generic.o $(BIN_DIR)/full_stream_graph.o $(BIN_DIR)/link_stream.o $(BIN_DIR)/stream_graph.o $(BIN_DIR)/events_table.o $(BIN_DIR)/key_moments_table.o $(BIN_DIR)/links_set.o $(BIN_DIR)/nodes_set.o $(BIN_DIR)/interval.o $(BIN_DIR)/bit_array.o