RANLIB ?= ranlib

CFLAGS += -ggdb

TARGETS = libnet.a tap.o
TARGETS += test

NET_OBJECTS = net.o tls.o bipbuf.o route.o netif.o net-util.o

TESTS = tls-cli tls-srv tap-tap route-list route-edit route-default-device
TESTS += netif-test netif-info netif-set

all: $(TARGETS)

clean:
	rm -f *.o $(TARGETS) $(TESTS)

PREFIX ?= /usr/local

install: $(TARGETS)
	install -D -d $(DESTDIR)/$(PREFIX)/bin
	install -s -m 0755 $^ $(DESTDIR)/$(PREFIX)/bin

libnet.a: $(NET_OBJECTS)
	$(AR) rc $@ $^
	$(RANLIB) $@

test: $(TESTS)

tls-cli: CFLAGS += `pkg-config gnutls --cflags --libs`
tls-cli: libnet.a

tls-srv: CFLAGS += `pkg-config gnutls --cflags --libs`
tls-srv: libnet.a

tap-tap: tap-tap.o tap.o

route-list: libnet.a
route-edit: libnet.a
route-default-device: libnet.a

netif-info: libnet.a
netif-set:  libnet.a
