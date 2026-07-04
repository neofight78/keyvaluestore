CC		= gcc
CFLAGS	= -Iinc -Wall -Wextra -g

SRC	= src/kv.c src/main.c
OUT	= bin/kv

.PHONY: run format

run: $(OUT)
	./$(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

format:
	clang-format -i $(SRC) inc/*.h

valgrind: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all ./$(OUT)
