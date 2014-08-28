CC = cc
CFLAGS = -Wall -O3

SRC = hid.c hidraw.c main.c uinput.c
OBJ = $(SRC:.c=.o)

ps3remote: $(OBJ)
	$(CC) $(CFLAGS) -ludev -o ps3remote $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f ps3remote $(OBJ)
