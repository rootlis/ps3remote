CC=clang
CFLAGS=-Wall -O3

ps3remote: main.o uinput.o hidraw.o rdescriptor.o
	$(CC) $(CFLAGS) -ludev -o ps3remote main.o uinput.o hidraw.o rdescriptor.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f ps3remote *.o
