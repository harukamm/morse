CC = clang++ -MMD -MP	-std=c++14

SOURCES=main.cc trie.cc training.cc util.cc
OBJS = $(SOURCES:.cc=.o)

all: main

main: $(OBJS)
	$(CC) -g -o $@ $(OBJS)

$(OBJS): %.o: %.cc
	$(CC) -c -o $@ $<

clean:
	rm -f main $(OBJS)

-include *.d

.PHONY: all clean

