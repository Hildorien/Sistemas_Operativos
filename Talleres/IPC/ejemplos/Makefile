.POSIX:
.SUFFIXES:
.SUFFIXES: .c

BIN = \
	pipe-0                   \
	pipe-1                   \
	socket-server-0-dgram    \
	socket-server-0-stream   \
	socket-client            \
	socket-server-1-blocking \
	socket-server-1-fcntl    \
	socket-server-1-pselect

SRC = $(BIN:=.c)

.c:
	$(CC) -D_POSIX_C_SOURCE=200809L $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

all: $(BIN)

clean:
	rm -f $(BIN)

dist:
	mkdir -p ejemplos-ipc
	cp -f Makefile $(SRC) ejemplos-ipc
	tar -cf ejemplos-ipc.tar ejemplos-ipc
	gzip ejemplos-ipc.tar
