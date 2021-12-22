CC = clang++ -std=c++2a
BINDIR = bin
SRCDIR = src
WARNINGS_FLAGS = -Wall -Wextra

SRC_FILES = $(wildcard $(SRCDIR)/*.cpp)

main:
	mkdir -p $(BINDIR)
	$(CC) -o $(BINDIR)/lum $(SRC_FILES) $(WARNINGS_FLAGS)

clean:
	rm -r $(BINDIR)
