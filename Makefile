SRCS=$(wildcard src/*.c)
DEPS=$(patsubst src/%,bin/deps/%.d,$(SRCS))
OBJS=$(patsubst src/%,bin/objs/%.o,$(SRCS))

CC=gcc
LD=gcc
CCFLAGS=-Wall -Wextra -g -ggdb -Isrc -Ilibz80 -c \
	`pkg-config --cflags gtk+-3.0` -Wno-unused-parameter
LDFLAGS=-g -ggdb `pkg-config --libs gtk+-3.0` -rdynamic -fsanitize=address

all: bin/vgtk

clean:
	cd libz80 && make clean
	rm -rf bin

-include $(DEPS)

libz80/libz80.so:
	cd libz80 && make

bin/vgtk: libz80/libz80.so $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

bin/objs/%.c.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -o $@ $<

bin/deps/%.c.d: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -M -o $@ $< -MT $(patsubst bin/deps/%,bin/objs/%,$@)

