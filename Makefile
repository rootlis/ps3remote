CC=clang
CFLAGS=-Wall -g

ps3remote: main.o
	$(CC) $(CFLAGS) -ludev -o ps3remote main.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm ps3remote *.o
