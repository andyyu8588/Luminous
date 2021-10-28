CC = clang++
BINDIR = bin
SRCDIR = src
WARNINGS_FLAGS = -Wall -Wextra

SRC_FILES = $(wildcard $(SRCDIR)/*.cpp)

setup:
	mkdir -p $(BINDIR)

main:
	$(CC) -o $(BINDIR)/lum $(SRC_FILES) $(WARNINGS_FLAGS)

clean:
	rm -r $(BINDIR)