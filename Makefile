CC		= gcc
CFLAGS	= -Iinc -Wall -Wextra

SRC	= src/kv.c src/main.c
OUT	= bin/kv

.PHONY: run

run: $(OUT)
	./$(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)
