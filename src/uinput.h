#ifndef UINPUT_H
#define UINPUT_H

#include <stdint.h>

enum uinput_keyvals {
	KEYUP	= 0,
	KEYDOWN	= 1,
};

int uinput_open (void);
int uinput_close (int fd);
int uinput_sendkey (int fd, uint16_t key, int32_t value);

#endif
