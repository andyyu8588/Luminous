default: debug

COMPILER = clang++ -std=c++2a

BIN_DIR = bin
SRC_DIR = src
TESTS_DIR = tests

TESTING_FLAGS = -g -DDEBUG
WARNINGS_FLAGS = -Wall -Wextra -Wstrict-prototypes

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

EXECUTABLE = luminous

FORMATTER = clang-format
FORMATTER_FLAGS = -i -style=Google
FORMATER_FILES = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/*.h)

format:
	$(FORMATTER) $(FORMATTER_FLAGS) $(FORMATER_FILES)

setup:
	$(MAKE) format
	mkdir -p $(BIN_DIR)

main:
	$(MAKE) setup
	$(COMPILER) -o $(BIN_DIR)/$(EXECUTABLE) $(SRC_FILES) $(WARNINGS_FLAGS) 

debug:
	$(MAKE) setup	
	$(COMPILER) -o $(BIN_DIR)/$(EXECUTABLE) $(SRC_FILES) $(WARNINGS_FLAGS) $(TESTING_FLAGS)
	gdb ./$(BIN_DIR)/$(EXECUTABLE)

basic:
	./$(BIN_DIR)/$(EXECUTABLE) ./$(TESTS_DIR)/basic.in

io:
	@bash ./io-test.sh

memory:
	@bash ./memory-test.sh

test:
	$(MAKE) main
	$(MAKE) io
	$(MAKE) memory

clean:
	rm -r $(BIN_DIR)

gdb:
	gdb ./$(BIN_DIR)/$(EXECUTABLE)

repl:
	./$(BIN_DIR)/$(EXECUTABLE) 
