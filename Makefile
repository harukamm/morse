CC = clang++ -MMD -MP	-std=c++14

SRC_DIR = ./src
SOURCES = $(shell ls $(SRC_DIR)/*.cc)
OBJS = $(SOURCES:.cc=.o)

all: main

main: $(OBJS)
	$(CC) -g -o $@ $(OBJS)

$(OBJS): %.o: %.cc
	$(CC) -c -o $@ $<

clean:
	rm -f main $(OBJS)
	rm -fr training/*

-include *.d

.PHONY: all clean

