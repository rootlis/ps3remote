#ifndef HIDRAW_H
#define HIDRAW_H

#include <libudev.h>
#include <stdint.h>

struct report {
	uint8_t id;
	uint8_t junk[3];
	uint8_t key;
	uint8_t effs[5];
	uint16_t press;
};


int open_hidraw (struct udev_device *dev);
uint16_t read_hidraw (int fd);

#endif
