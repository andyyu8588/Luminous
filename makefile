default: debug

CC = clang++ -std=c++2a

BINDIR = bin
SRCDIR = src
TESTSDIR = tests

TESTING_FLAGS = -g -DDEBUG
WARNINGS_FLAGS = -Wall -Wextra

EXECUTABLE = luminous

FORMATTER = clang-format
FORMATTER_STYLE = Google

SRC_FILES = $(wildcard $(SRCDIR)/*.cpp)
FORMATER_FILES = $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.h)

setup:
	$(FORMATTER) -i -style=$(FORMATTER_STYLE) $(FORMATER_FILES)
	mkdir -p $(BINDIR)

main:
	$(MAKE) setup
	$(CC) -o $(BINDIR)/$(EXECUTABLE) $(SRC_FILES) $(WARNINGS_FLAGS) 

debug:
	$(MAKE) setup	
	$(CC) -o $(BINDIR)/$(EXECUTABLE) $(SRC_FILES) $(WARNINGS_FLAGS) $(TESTING_FLAGS)
	gdb ./$(BINDIR)/$(EXECUTABLE)

basic:
	./$(BINDIR)/$(EXECUTABLE) ./$(TESTSDIR)/basic.lum

clean:
	rm -r $(BINDIR)

repl:
	./$(BINDIR)/$(EXECUTABLE) 
