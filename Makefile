SRCS=$(wildcard src/*.c)
DEPS=$(patsubst %,bin/deps/%.d,$(SRCS))
OBJS=$(patsubst %,bin/objs/%.o,$(SRCS))

CC=gcc
LD=gcc
CCFLAGS=-Wall -Wextra -g -ggdb -Isrc -Ilibz80 -c \
	`pkg-config --cflags gtk+-3.0` -Wno-unused-parameter \
	-std=c89
LDFLAGS=-g -ggdb `pkg-config --libs gtk+-3.0` -rdynamic -fsanitize=address

all: bin/vgtk

clean:
	cd libz80 && make clean
	rm -rf bin

ifneq ($(MAKECMDGOALS), clean)
-include $(DEPS)
endif

libz80/libz80.so:
	cd libz80 && make

bin/vgtk: libz80/libz80.so $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

bin/objs/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -o $@ $<

bin/deps/%.c.d: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) -M -o $@ $< -MT $(patsubst bin/deps/%.d,bin/objs/%.o,$@)

