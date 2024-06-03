CC = gcc
SRC_DIR = src
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3
FLAGS = $(RELEASE_FLAGS)
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic $(FLAGS)
LDFLAGS = -lm

interval:
	$(CC) $(CFLAGS) -c -o interval.o $(SRC_DIR)/interval.c $(LDFLAGS)
