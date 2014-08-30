#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include "uinput.h"
#include "remote.h"
#include "utils.h"


static void
debug_event (struct input_event *ev)
{
	int size;
	int i;
	size = sizeof(struct input_event);
	printf("=>");
	for (i=size - 2; i>=0; i-=2) {
		printf(" 0x%04x", *((uint16_t*)((uint8_t*)ev+i)));
	}
	putchar('\n');
}


static int
uinput_write (int fd, struct input_event *ev, size_t size)
{
	ssize_t bytes;
	if ((bytes = write(fd, ev, size)) < size) {
		if (bytes < 0) {
			perror("write");
			return -1;
		}
		fprintf(stderr, "uinput_write: unable to write entire struct "
			"(sent %zd of %zd bytes)\n", bytes, size);
		return -1;
	}
	return 0;
}


int
uinput_open (void) 
{
	int fd;
	struct uinput_user_dev uidev;
	if ((fd = open("/dev/uinput", O_WRONLY)) < 0) {
		perror("uinput_open");
		return -1;
	}
	if (ioctl(fd, UI_SET_EVBIT, EV_SYN) < 0) {
		perror("UI_SET_EVBIT");
		goto uinput_open_error;
	}
	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0) {
		perror("UI_SET_EVBIT");
		goto uinput_open_error;
	}

	/* 
	 * KEY_OK needs to be set for XBMC to receive any keypresses. I don't
	 * know why this is necessary. From what I can tell, any keybit at or
	 * above KEY_OK will work, but anything lower will not. I haven't done
	 * thorough testing, though, so this could be wrong.
	 */
	if (ioctl(fd, UI_SET_KEYBIT, KEY_OK) < 0) goto keybit_error;

	if (ioctl(fd, UI_SET_KEYBIT, KEY_FASTFORWARD)  < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_NEXTSONG)     < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_PAUSE)        < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_PLAY)         < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_PREVIOUSSONG) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_REWIND)       < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_STOP)         < 0) goto keybit_error;

	if (ioctl(fd, UI_SET_KEYBIT, KEY_ENTER) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_UP)    < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_RIGHT) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_DOWN)  < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_LEFT)  < 0) goto keybit_error;

	if (ioctl(fd, UI_SET_KEYBIT, KEY_0) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_1) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_2) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_3) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_4) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_5) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_6) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_7) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_8) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_9) < 0) goto keybit_error;

	if (ioctl(fd, UI_SET_KEYBIT, KEY_A) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_B) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_D) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_G) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_H) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_I) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_L) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_M) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_N) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_O) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_P) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_R) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_S) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_U) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_V) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_X) < 0) goto keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_Y) < 0) goto keybit_error;

	memset(&uidev, 0, sizeof uidev);
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, UREMOTE_NAME);
	uidev.id.bustype = BUS_VIRTUAL;
	uidev.id.vendor  = VENDOR_ID;
	uidev.id.product = PRODUCT_ID;
	uidev.id.version = 1;
	if (write(fd, &uidev, sizeof(uidev)) < 0) {
		perror("write");
		goto uinput_open_error;
	}
	if (ioctl(fd, UI_DEV_CREATE) < 0) {
		perror("UI_DEV_CREATE");
		goto uinput_open_error;
	}

	return fd;

keybit_error:
	perror("UI_SET_KEYBIT");
uinput_open_error:
	close(fd);
	return -1;
}


int
uinput_close (int fd)
{
	if (ioctl(fd, UI_DEV_DESTROY) < 0) {
		perror("UI_DEV_DESTROY");
	}
	return close(fd);
}


int
uinput_sendkey (int fd, uint16_t key, int32_t value)
{
	struct input_event ev[2];

	memset(&ev, 0, sizeof ev);
	ev[0].type = EV_KEY;
	ev[0].code = key;
	ev[0].value = value;
	ev[1].type = EV_SYN;
	ev[1].code = SYN_REPORT;

	debug_puts("Sending EV_KEY event to uinput");
	DEBUG_FN(debug_event(&(ev[0])));
	uinput_write(fd, &ev[0], sizeof ev[0]);
	debug_puts("Sending EV_SYN event to uinput");
	DEBUG_FN(debug_event(&(ev[1])));
	uinput_write(fd, &ev[1], sizeof ev[1]);
	return 0;

	/*
	 * Unworking code, sends the EV_KEY and EV_SYN together. On RPi, only
	 * every eigth EV_SYN is received.
	 */
	uinput_write(fd, ev, sizeof ev);
	return 0;
}
