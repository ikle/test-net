CFLAGS += -ggdb

TARGETS = net.o tls.o
TARGETS += test

TESTS = tls-cli tls-srv

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS) $(TESTS)

PREFIX ?= /usr/local

install: $(TARGETS)
	install -D -d $(DESTDIR)/$(PREFIX)/bin
	install -s -m 0755 $^ $(DESTDIR)/$(PREFIX)/bin

test: $(TESTS)

tls-cli: CFLAGS += `pkg-config gnutls --cflags --libs`
tls-cli: net.o tls.o

tls-srv: CFLAGS += `pkg-config gnutls --cflags --libs`
tls-srv: net.o tls.o
