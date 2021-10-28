CC = clang++
BINDIR = bin
SRCDIR = src
WARNINGS_FLAGS = -Wall -Wextra

setup:
	mkdir -p $(BINDIR)

main:
	$(CC) -o $(BINDIR)/lum $(SRCDIR)/main.o $(WARNINGS_FLAGS)

clean:
	rm -r $(BINDIR)