CC = clang++ -std=c++2a
BINDIR = bin
SRCDIR = src
WARNINGS_FLAGS = -Wall -Wextra
EXECUTABLE = luminous

SRC_FILES = $(wildcard $(SRCDIR)/*.cpp)

FORMATTER = clang-format
FORMATTER_STYLE = Google
FORMATER_FILES = $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/*.h)

main:
	mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/$(EXECUTABLE) $(SRC_FILES) $(WARNINGS_FLAGS)

clean:
	rm -r $(BINDIR)

format:
	$(FORMATTER) -i -style=$(FORMATTER_STYLE) $(FORMATER_FILES)

run:
	./$(BINDIR)/$(EXECUTABLE)
