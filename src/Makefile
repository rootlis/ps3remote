CC = /usr/bin/gcc
CFLAGS = -Wall -O3
INSTALL = /usr/bin/install
PREFIX = /usr

src = hid.c hidraw.c main.c uinput.c
obj = $(src:.c=.o)

# Install stuff
service = systemd/ps3remote.service
systemd = /usr/lib/systemd/system
bin = $(PREFIX)/bin

ps3remote: $(obj)
	$(CC) $(CFLAGS) -ludev -o ps3remote $(obj)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

install: ps3remote
	$(INSTALL) -o root -m 755 ps3remote $(bin)
	$(INSTALL) -o root -m 644 $(service) $(systemd)

uninstall:
	rm -f $(bin)/ps3remote 
	rm -f $(systemd)/$(service)

clean:
	rm -f ps3remote $(obj)

.PHONY: all clean install uninstall
