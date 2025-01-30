CC ?= gcc
DEBUG_FLAGS = -g -O0 -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer -fsanitize=undefined
RELEASE_FLAGS = -O4 -march=native -flto -DNDEBUG
BENCHMARK_FLAGS = -O3 -g
CFLAGS = -Wall -Wextra -std=c2x
LDFLAGS = -lm

UTILITIES_BIN_DIR = utilities/bin
EXAMPLES_BIN_DIR = examples/bin

compile_mode ?= release

# Check what compile mode is set, and set the flags and binary directory accordingly
ifeq ($(compile_mode), debug)
	FLAGS = $(DEBUG_FLAGS)
	LDFLAGS += -fsanitize=address,undefined
	BIN_DIR = bin/debug
else ifeq ($(compile_mode), release)
	FLAGS = $(RELEASE_FLAGS)
	BIN_DIR = bin/release
else ifeq ($(compile_mode), benchmark)
	FLAGS = $(BENCHMARK_FLAGS)
	BIN_DIR = bin/benchmark
endif



# Ensure the binary directory exists
bin:
	mkdir -p bin/
	mkdir -p bin/debug
	mkdir -p bin/release
	mkdir -p bin/benchmark
	mkdir -p $(UTILITIES_BIN_DIR)
	mkdir -p $(EXAMPLES_BIN_DIR)

utils.o: src/utils.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@
	
defaults.o: src/defaults.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

units.o: src/units.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

bit_array.o: src/bit_array.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

interval.o: src/interval.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

stream.o: src/stream.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

iterators.o: src/iterators.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

stream_functions.o: src/stream_functions.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

arena.o: src/arena.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@


events_table.o: src/stream_graph/events_table.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

key_instants_table.o: src/stream_graph/key_instants_table.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

key_instants_access.o: src/stream_data_access/key_instants.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

links_set.o: src/stream_graph/links_set.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

nodes_set.o: src/stream_graph/nodes_set.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@


full_stream_graph.o: src/streams/full_stream_graph.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

link_stream.o: src/streams/link_stream.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

chunk_stream.o: src/streams/chunk_stream.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

chunk_stream_small.o: src/streams/chunk_stream_small.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

timeframe_stream.o: src/streams/timeframe_stream.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@


node_access.o: src/stream_data_access/node_access.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

link_access.o: src/stream_data_access/link_access.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

induced_graph.o: src/stream_data_access/induced_graph.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@


metrics.o: src/analysis/metrics.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

cluster.o: src/analysis/cluster.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@
	
cliques.o: src/analysis/cliques.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

kcores.o: src/analysis/kcores.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

isomorphism.o: src/analysis/isomorphism.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

walks.o: src/analysis/walks.c | bin
	$(CC) $(CFLAGS) $(FLAGS) -c $< -o $(BIN_DIR)/$@

libSGA: utils.o defaults.o units.o bit_array.o interval.o events_table.o key_instants_table.o links_set.o nodes_set.o stream.o node_access.o link_access.o induced_graph.o full_stream_graph.o link_stream.o chunk_stream.o chunk_stream_small.o timeframe_stream.o iterators.o metrics.o cluster.o arena.o walks.o cliques.o kcores.o isomorphism.o stream_functions.o key_instants_access.o | bin
	gcc-ar rcs $(BIN_DIR)/libSGA.a $(foreach obj,$^,$(BIN_DIR)/$(obj))

docs:
	doxygen .doxygen

clean:
	rm -rf bin/**
	rm -rf $(UTILITIES_BIN_DIR)/**
	rm -rf $(EXAMPLES_BIN_DIR)/**
	rmdir bin
	rmdir $(UTILITIES_BIN_DIR)
	rmdir $(EXAMPLES_BIN_DIR)

utilities: utilities/external_to_internal.c libSGA | bin
	$(CC) $(CFLAGS) $(FLAGS) $< -o $(UTILITIES_BIN_DIR)/external_to_internal -L$(BIN_DIR) -lSGA -lm

examples: examples/split.c examples/metrics.c examples/analysis.c examples/time_evolution.c libSGA | bin
	$(CC) $(CFLAGS) $(FLAGS) examples/split.c -o $(EXAMPLES_BIN_DIR)/split -L$(BIN_DIR) -lSGA -lm
	$(CC) $(CFLAGS) $(FLAGS) examples/metrics.c -o $(EXAMPLES_BIN_DIR)/metrics -L$(BIN_DIR) -lSGA -lm
	$(CC) $(CFLAGS) $(FLAGS) examples/analysis.c -o $(EXAMPLES_BIN_DIR)/analysis -L$(BIN_DIR) -lSGA -lm
	$(CC) $(CFLAGS) $(FLAGS) examples/time_evolution.c -o $(EXAMPLES_BIN_DIR)/time_evolution -L$(BIN_DIR) -lSGA -lm