CC=clang
CFLAGS=-Wall -g

ps3remote: remote.o uinput.o hidraw.o rdescriptor.o
	$(CC) $(CFLAGS) -ludev -o ps3remote remote.o uinput.o hidraw.o rdescriptor.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm ps3remote *.o
