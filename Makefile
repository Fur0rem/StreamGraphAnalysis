CC = gcc
SRC_DIR = src
BIN_DIR = bin
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3
FLAGS = $(RELEASE_FLAGS)
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic $(FLAGS)
LDFLAGS = -lm

interval:
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)/interval.o $(SRC_DIR)/interval.c $(LDFLAGS)
