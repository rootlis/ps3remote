CC=clang
CFLAGS=-Wall -O3

ps3remote: main.o uinput.o hidraw.o hid.o
	$(CC) $(CFLAGS) -ludev -o ps3remote main.o uinput.o hidraw.o hid.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f ps3remote *.o
