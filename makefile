default: debug

CC = clang++ -std=c++2a
BINDIR = bin
SRCDIR = src
TESTING_FLAGS = -g -DDEBUG
WARNINGS_FLAGS = -Wall -Wextra
EXECUTABLE = luminous

SRC_FILES = $(wildcard $(SRCDIR)/*.cpp)

FORMATTER = clang-format
FORMATTER_STYLE = Google
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

clean:
	rm -r $(BINDIR)

run:
	./$(BINDIR)/$(EXECUTABLE) 
