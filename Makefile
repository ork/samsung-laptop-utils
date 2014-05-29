CC        = gcc
SOURCES   = $(wildcard *.c)
BINARIES  = $(SOURCES:%.c=%)

CFLAGS    = -std=c99 -O3 -Wall -Wextra -Wpedantic -Wstrict-aliasing
CFLAGS   += $(shell pkg-config --cflags glib-2.0 gio-2.0 libnotify)
LDFLAGS   = $(shell pkg-config --libs   glib-2.0 gio-2.0 libnotify)

all: $(BINARIES)

${BINARIES}: ${SOURCES}
	$(CC) $(CFLAGS) $(LDFLAGS) $@.c -o $@

.PHONY: clean mrproper

clean:
	rm -rf $(BINARIES)
