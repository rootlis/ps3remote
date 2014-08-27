CC=clang
CFLAGS=-Wall -g

ps3remote: main.o uinput.o hidraw.o rdescriptor.o
	$(CC) $(CFLAGS) -ludev -o ps3remote remote.o uinput.o hidraw.o rdescriptor.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f ps3remote *.o
