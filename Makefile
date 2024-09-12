CC = gcc
DEBUG_FLAGS = -g -O0 -DDEBUG
RELEASE_FLAGS = -O4 -lto -march=native -flto -DNDEBUG
BENCHMARK_FLAGS = -O3 -g
FLAGS = $(RELEASE_FLAGS)
CFLAGS = -Wall -Wextra $(FLAGS) -std=c2x
LDFLAGS = -lm

# Ensure the binary directory exists

bin:
	mkdir -p bin

utils: src/utils.c | bin
	$(CC) $(CFLAGS) -c $< -o bin/utils.o

defaults: src/defaults.c utils | bin
	$(CC) $(CFLAGS) -c src/defaults.c -o bin/defaults.o
	ar rcs bin/defaults.a bin/defaults.o bin/utils.o

units: src/units.c defaults | bin
	$(CC) $(CFLAGS) -c src/units.c -o bin/units.o
	ar rcs bin/units.a bin/units.o bin/defaults.a

bit_array: src/bit_array.c utils | bin
	$(CC) $(CFLAGS) -c $< -o bin/bit_array.o
	ar rcs bin/bit_array.a bin/bit_array.o bin/utils.o

interval: src/interval.c defaults utils | bin
	$(CC) $(CFLAGS) -c src/interval.c -o bin/interval.o
	ar rcs bin/interval.a bin/interval.o bin/defaults.o bin/utils.o

events_table: src/stream_graph/events_table.c defaults | bin
	$(CC) $(CFLAGS) -c $< -o bin/events_table.o
	ar rcs bin/events_table.a bin/events_table.o bin/defaults.a

key_moments_table: src/stream_graph/key_moments_table.c defaults | bin
	$(CC) $(CFLAGS) -c $< -o bin/key_moments_table.o
	ar rcs bin/key_moments_table.a bin/key_moments_table.o bin/defaults.a

links_set: src/stream_graph/links_set.c defaults | bin
	$(CC) $(CFLAGS) -c $< -o bin/links_set.o
	ar rcs bin/links_set.a bin/links_set.o bin/defaults.a

nodes_set: src/stream_graph/nodes_set.c defaults | bin
	$(CC) $(CFLAGS) -c $< -o bin/nodes_set.o
	ar rcs bin/nodes_set.a bin/nodes_set.o bin/defaults.a

stream: src/stream.c events_table key_moments_table links_set nodes_set defaults interval utils bit_array | bin
	$(CC) $(CFLAGS) -c src/stream.c -o bin/stream.o
	ar rcs bin/stream.a bin/stream.o bin/events_table.o bin/key_moments_table.o bin/links_set.o bin/nodes_set.o bin/defaults.o bin/interval.o bin/utils.o bin/bit_array.o

node_access: src/stream_data_access/node_access.c interval stream defaults events_table key_moments_table links_set nodes_set bit_array units utils| bin
	$(CC) $(CFLAGS) -c src/stream_data_access/node_access.c -o bin/node_access.o
	ar rcs bin/node_access.a bin/interval.o bin/stream.o bin/node_access.o bin/defaults.o bin/interval.o bin/utils.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/units.o

link_access: src/stream_data_access/link_access.c interval stream defaults events_table key_moments_table links_set nodes_set bit_array units utils| bin
	$(CC) $(CFLAGS) -c src/stream_data_access/link_access.c -o bin/link_access.o
	ar rcs bin/link_access.a bin/interval.o bin/stream.o bin/link_access.o bin/defaults.o bin/interval.o bin/utils.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/units.o

induced_graph: src/stream_data_access/induced_graph.c interval stream defaults events_table key_moments_table links_set nodes_set bit_array units utils| bin
	$(CC) $(CFLAGS) -c src/stream_data_access/induced_graph.c -o bin/induced_graph.o
	ar rcs bin/induced_graph.a bin/interval.o bin/induced_graph.o bin/defaults.o bin/interval.o bin/stream.o bin/events_table.o bin/key_moments_table.o bin/links_set.o bin/nodes_set.o bin/bit_array.o bin/units.o bin/utils.o

# full_stream_graph: src/stream_wrappers/full_stream_graph.c interval stream defaults | bin
# 	$(CC) $(CFLAGS) -c src/stream_wrappers/full_stream_graph.c -o bin/full_stream_graph.o
# 	ar rcs bin/full_stream_graph.a bin/interval.a bin/stream.a bin/full_stream_graph.o bin/defaults.a

full_stream_graph: src/stream_wrappers/full_stream_graph.c interval stream defaults units induced_graph events_table key_moments_table links_set nodes_set utils bit_array node_access link_access | bin
	$(CC) $(CFLAGS) -c src/stream_wrappers/full_stream_graph.c -o bin/full_stream_graph.o
	ar rcs bin/full_stream_graph.a bin/full_stream_graph.o bin/events_table.o bin/stream.o bin/key_moments_table.o bin/interval.o bin/utils.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/defaults.o bin/induced_graph.o bin/events_table.o bin/stream.o bin/units.o bin/node_access.o bin/link_access.o

# TODO : idfk how to do proper makefile dependencies
link_stream: src/stream_wrappers/link_stream.c interval stream defaults units induced_graph events_table key_moments_table links_set nodes_set utils bit_array iterators full_stream_graph node_access link_access iterators | bin
	$(CC) $(CFLAGS) -c src/stream_wrappers/link_stream.c -o bin/link_stream.o
	ar rcs bin/link_stream.a bin/link_stream.o bin/events_table.o bin/stream.o bin/key_moments_table.o bin/interval.o bin/utils.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/defaults.o bin/induced_graph.o bin/events_table.o bin/stream.o bin/units.o bin/iterators.o bin/node_access.o bin/link_access.o 

chunk_stream: src/stream_wrappers/chunk_stream.c interval stream defaults units induced_graph events_table key_moments_table links_set nodes_set utils bit_array full_stream_graph iterators node_access link_access | bin
	$(CC) $(CFLAGS) -c src/stream_wrappers/chunk_stream.c -o bin/chunk_stream.o
	ar rcs bin/chunk_stream.a bin/chunk_stream.o bin/events_table.o bin/stream.o bin/key_moments_table.o bin/interval.o bin/utils.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/defaults.o bin/induced_graph.o bin/events_table.o bin/stream.o bin/units.o bin/full_stream_graph.o bin/iterators.o bin/node_access.o bin/link_access.o

chunk_stream_small: src/stream_wrappers/chunk_stream_small.c interval stream defaults units induced_graph events_table key_moments_table links_set nodes_set utils bit_array full_stream_graph iterators node_access link_access | bin
	$(CC) $(CFLAGS) -c src/stream_wrappers/chunk_stream_small.c -o bin/chunk_stream_small.o
	ar rcs bin/chunk_stream_small.a bin/chunk_stream_small.o bin/events_table.o bin/stream.o bin/key_moments_table.o bin/interval.o bin/utils.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/defaults.o bin/induced_graph.o bin/events_table.o bin/stream.o bin/units.o bin/full_stream_graph.o bin/iterators.o bin/node_access.o bin/link_access.o

snapshot_stream: src/stream_wrappers/snapshot_stream.c interval stream defaults units induced_graph events_table key_moments_table links_set nodes_set utils bit_array iterators full_stream_graph iterators node_access link_access | bin	
	$(CC) $(CFLAGS) -c src/stream_wrappers/snapshot_stream.c -o bin/snapshot_stream.o
	$(CC) $(CFLAGS) -c src/metrics.c -o bin/metrics.o
	$(CC) $(CFLAGS) -c src/stream_wrappers/chunk_stream.c -o bin/chunk_stream.o
	$(CC) $(CFLAGS) -c src/stream_wrappers/chunk_stream_small.c -o bin/chunk_stream_small.o
	$(CC) $(CFLAGS) -c src/stream_wrappers/link_stream.c -o bin/link_stream.o
	ar rcs bin/snapshot_stream.a bin/snapshot_stream.o bin/metrics.o bin/events_table.o bin/stream.o bin/key_moments_table.o bin/interval.o bin/utils.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/defaults.o bin/induced_graph.o bin/events_table.o bin/stream.o bin/units.o bin/iterators.o bin/full_stream_graph.o bin/chunk_stream.o bin/chunk_stream_small.o bin/interval.o bin/link_stream.o bin/iterators.o bin/node_access.o bin/link_access.o

iterators: src/iterators.c | bin
	$(CC) $(CFLAGS) -c $< -o bin/iterators.o

metrics: src/metrics.c stream interval defaults utils full_stream_graph link_stream chunk_stream chunk_stream_small bit_array nodes_set links_set events_table key_moments_table induced_graph interval iterators units snapshot_stream node_access link_access | bin
	$(CC) $(CFLAGS) -c src/metrics.c -o bin/metrics.o
	ar rcs bin/metrics.a bin/metrics.o bin/stream.o bin/interval.o bin/defaults.o bin/utils.o bin/full_stream_graph.o bin/link_stream.o bin/chunk_stream.o bin/chunk_stream_small.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/events_table.o bin/key_moments_table.o bin/induced_graph.o bin/interval.o bin/iterators.o bin/defaults.o bin/utils.o bin/units.o bin/snapshot_stream.o bin/node_access.o bin/link_access.o

arena:
	$(CC) $(CFLAGS) -c src/arena.c -o bin/arena.o
	ar rcs bin/arena.a bin/arena.o

walks: src/analysis/walks.c stream interval defaults utils full_stream_graph link_stream chunk_stream chunk_stream_small bit_array nodes_set links_set events_table key_moments_table induced_graph interval iterators units snapshot_stream arena | bin
	$(CC) $(CFLAGS) -c src/analysis/walks.c -o bin/walks.o
	ar rcs bin/walks.a bin/walks.o bin/stream.o bin/interval.o bin/defaults.o bin/utils.o bin/full_stream_graph.o bin/link_stream.o bin/chunk_stream.o bin/chunk_stream_small.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/events_table.o bin/key_moments_table.o bin/induced_graph.o bin/interval.o bin/iterators.o bin/defaults.o bin/utils.o bin/units.o bin/snapshot_stream.o bin/arena.o bin/metrics.o bin/node_access.o bin/link_access.o

cliques: src/analysis/cliques.c stream interval defaults utils full_stream_graph link_stream chunk_stream chunk_stream_small bit_array nodes_set links_set events_table key_moments_table induced_graph interval iterators units snapshot_stream metrics | bin
	$(CC) $(CFLAGS) -c src/analysis/cliques.c -o bin/cliques.o
	ar rcs bin/cliques.a bin/cliques.o bin/stream.o bin/interval.o bin/defaults.o bin/utils.o bin/full_stream_graph.o bin/link_stream.o bin/chunk_stream.o bin/chunk_stream_small.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/events_table.o bin/key_moments_table.o bin/induced_graph.o bin/interval.o bin/iterators.o bin/defaults.o bin/utils.o bin/units.o bin/snapshot_stream.o bin/metrics.o bin/node_access.o bin/link_access.o

kcores: src/analysis/kcores.c stream interval defaults utils full_stream_graph link_stream chunk_stream chunk_stream_small bit_array nodes_set links_set events_table key_moments_table induced_graph interval iterators units snapshot_stream metrics | bin
	$(CC) $(CFLAGS) -c src/analysis/kcores.c -o bin/kcores.o
	ar rcs bin/kcores.a bin/kcores.o bin/stream.o bin/interval.o bin/defaults.o bin/utils.o bin/full_stream_graph.o bin/link_stream.o bin/chunk_stream.o bin/chunk_stream_small.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/events_table.o bin/key_moments_table.o bin/induced_graph.o bin/interval.o bin/iterators.o bin/defaults.o bin/utils.o bin/units.o bin/snapshot_stream.o bin/metrics.o bin/node_access.o bin/link_access.o

isomorphism: src/analysis/isomorphism.c stream interval defaults utils full_stream_graph link_stream chunk_stream chunk_stream_small bit_array nodes_set links_set events_table key_moments_table induced_graph interval iterators units snapshot_stream metrics node_access link_access | bin
	$(CC) $(CFLAGS) -c src/analysis/isomorphism.c -o bin/isomorphism.o
	ar rcs bin/isomorphism.a bin/isomorphism.o bin/stream.o bin/interval.o bin/defaults.o bin/utils.o bin/full_stream_graph.o bin/link_stream.o bin/chunk_stream.o bin/chunk_stream_small.o bin/bit_array.o bin/nodes_set.o bin/links_set.o bin/events_table.o bin/key_moments_table.o bin/induced_graph.o bin/interval.o bin/iterators.o bin/defaults.o bin/utils.o bin/units.o bin/snapshot_stream.o bin/metrics.o bin/node_access.o bin/link_access.o

clean:
	rm -f bin/*

# TODO : whats the proper way to do this
libSGA: cliques walks arena iterators utils units defaults interval events_table key_moments_table links_set nodes_set stream induced_graph full_stream_graph link_stream chunk_stream chunk_stream_small metrics bit_array snapshot_stream kcores | bin
	ar rcs bin/libSGA.a bin/cliques.o bin/walks.o bin/arena.o bin/iterators.o bin/utils.o bin/units.o bin/defaults.o bin/interval.o bin/events_table.o bin/key_moments_table.o bin/links_set.o bin/nodes_set.o bin/stream.o bin/induced_graph.o bin/full_stream_graph.o bin/link_stream.o bin/chunk_stream.o bin/chunk_stream_small.o bin/metrics.o bin/bit_array.o bin/snapshot_stream.o bin/kcores.o

docs:
	doxygen .doxygen