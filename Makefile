CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
BIN := build/forc
SRC := src/main.c

.PHONY: all test clean

all: $(BIN)

$(BIN): $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

test: $(BIN)
	@./$(BIN) | grep -q "ForC M0"
	@./$(BIN) PING | grep -q "PONG"
	@echo "ForC smoke test: PASS"

clean:
	rm -rf build
