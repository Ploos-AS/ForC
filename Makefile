CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
BIN := build/forc
TEST_IRC := build/test_irc
TEST_DISPATCHER := build/test_dispatcher
TEST_EVENTS := build/test_events
TEST_RUNTIME := build/test_runtime
TEST_FORTH_VM := build/test_forth_vm
SRC := src/main.c src/irc_core.c src/dispatcher.c src/events.c src/runtime_adapter.c src/forth_vm.c

.PHONY: all test clean
all: $(BIN)
$(BIN): $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)
$(TEST_IRC): tests/test_irc.c src/irc_core.c src/irc_core.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_irc.c src/irc_core.c -o $(TEST_IRC)
$(TEST_DISPATCHER): tests/test_dispatcher.c src/irc_core.c src/dispatcher.c src/irc_core.h src/dispatcher.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_dispatcher.c src/irc_core.c src/dispatcher.c -o $(TEST_DISPATCHER)
$(TEST_EVENTS): tests/test_events.c src/irc_core.c src/events.c src/irc_core.h src/events.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_events.c src/irc_core.c src/events.c -o $(TEST_EVENTS)
$(TEST_RUNTIME): tests/test_runtime.c src/runtime_adapter.c src/runtime_adapter.h src/forth_vm.c src/forth_vm.h src/irc_core.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_runtime.c src/runtime_adapter.c src/forth_vm.c -o $(TEST_RUNTIME)
$(TEST_FORTH_VM): tests/test_forth_vm.c src/forth_vm.c src/forth_vm.h
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_forth_vm.c src/forth_vm.c -o $(TEST_FORTH_VM)
test: $(BIN) $(TEST_IRC) $(TEST_DISPATCHER) $(TEST_EVENTS) $(TEST_RUNTIME) $(TEST_FORTH_VM)
	@./$(BIN) | grep -q "ForC M0"
	@./$(TEST_IRC)
	@./$(TEST_DISPATCHER)
	@./$(TEST_EVENTS)
	@./$(TEST_RUNTIME)
	@./$(TEST_FORTH_VM)
	@echo "ForC M1 tests: PASS"
clean:
	rm -rf build
