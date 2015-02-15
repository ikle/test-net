CFLAGS += -ggdb

TARGETS = net.o tls.o tap.o bipbuf.o route.o netif.o net-util.o
TARGETS += test

TESTS = tls-cli tls-srv tap-tap route-list route-edit route-default-device
TESTS += netif-test netif-info netif-set

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

tap-tap: tap-tap.o tap.o

route-list: route.o
route-edit: route.o
route-default-device: route.o

netif-info: netif.o
netif-set:  netif.o
