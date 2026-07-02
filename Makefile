CC		= gcc
CFLAGS	= -Iinc -Wall -Wextra

SRC	= src/kv.c src/main.c
OUT	= bin/kv

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

run: $(OUT)
	./$(OUT)
