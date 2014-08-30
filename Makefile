# Config
CC = /usr/bin/gcc
CFLAGS = -Wall
INSTALL = /usr/bin/install
PREFIX = /usr
XBMC = /var/lib/xbmc/.xbmc/userdata/keymaps

# Install stuff
keymap = xbmc/keyboard.xml
service = systemd/ps3remote.service
systemd = /usr/lib/systemd/system
bin = $(PREFIX)/bin

LDLIBS = -ludev
srcs = hid.c hidraw.c main.c uinput.c
srcs := $(addprefix src/,$(srcs))
objs = $(srcs:.c=.o)
target = ps3remote

# Rules
all: CFLAGS += -DDEBUG=0 -O3
all: $(target)

debug: CFLAGS += -g -DDEBUG -Og
debug: $(target)

install: $(target)
	$(INSTALL) -o root -m 755 $(target) $(bin)
	$(INSTALL) -o root -m 644 $(service) $(systemd)

installxbmc: install
	$(INSTALL) -o xbmc -m 644 $(keymap) $(XBMC)

uninstall:
	rm -f $(bin)/$(target)
	rm -f $(systemd)/$(service)

clean:
	$(RM) $(target) $(objs)

$(target): $(objs)
	$(CC) $(CFLAGS) $(LDLIBS) $^ -o $(target)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean debug install installxbmc uninstall
