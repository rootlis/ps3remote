CC=clang
CFLAGS=-Wall -ludev -g

ps3remote: main.o
	$(CC) $(CFLAGS) -o ps3remote main.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm ps3remote *.o
