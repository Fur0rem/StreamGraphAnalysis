CC = gcc
SRC_DIR = src
BIN_DIR = bin
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3
FLAGS = $(DEBUG_FLAGS)
CFLAGS = -Wall -Wextra $(FLAGS) -Wno-unused-function -std=c2x
LDFLAGS = -lm

bit_array: $(SRC_DIR)/bit_array.c
	$(CC) $(CFLAGS) -c $^ -o $(BIN_DIR)/$@.o

interval: $(SRC_DIR)/interval.c
	$(CC) $(CFLAGS) -c $^ -o $(BIN_DIR)/$@.o

stream: $(SRC_DIR)/stream.c $(SRC_DIR)/stream_graph/*.c interval
	$(CC) $(CFLAGS) -c src/stream.c src/stream_graph/*.c interval.o -o $(BIN_DIR)/stream.o
	ar rcs $(BIN_DIR)/stream.a $(BIN_DIR)/stream.o

full_stream_graph: $(SRC_DIR)/stream/full_stream_graph.c interval stream
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/$@.o
	ar rcs $(BIN_DIR)/full_stream_graph.a $(BIN_DIR)/interval.o $(BIN_DIR)/stream.a $(BIN_DIR)/$@.o

link_stream: $(SRC_DIR)/link_stream.c
	$(CC) $(CFLAGS) -c $^ -o $(BIN_DIR)/$@.o

chunk_stream: $(SRC_DIR)/chunk_stream.c
	$(CC) $(CFLAGS) -c $^ -o $(BIN_DIR)/$@.o

chunk_stream_small: $(SRC_DIR)/chunk_stream_small.c
	$(CC) $(CFLAGS) -c $^ -o $(BIN_DIR)/$@.o

clean:
	rm -f $(BIN_DIR)/*