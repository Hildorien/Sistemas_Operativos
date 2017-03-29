CC = c99

BIN = hello launch

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

all: $(BIN)

clean:
	rm -f $(BIN)
