#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/hidraw.h>

#include "hid.h"
#include "hidraw.h"
#include "remote.h"
#include "utils.h"


/* Keymaps */
static const char *ps3remote_keymap_remote_strings[] = {
	[0x00] = "KEY_1",
	[0x01] = "KEY_2",
	[0x02] = "KEY_3",
	[0x03] = "KEY_4",
	[0x04] = "KEY_5",
	[0x05] = "KEY_6",
	[0x06] = "KEY_7",
	[0x07] = "KEY_8",
	[0x08] = "KEY_9",
	[0x09] = "KEY_0",
	[0x0b] = "KEY_ENTER",		/* center ENTER */
	[0x0e] = "KEY_ESC",		/* return */
	[0x0f] = "KEY_CLEAR",
	[0x16] = "KEY_EJECTCD",
	[0x1a] = "KEY_MENU",		/* top menu */
	[0x28] = "KEY_TIME",
	[0x30] = "KEY_PREVIOUS",
	[0x31] = "KEY_NEXT",
	[0x32] = "KEY_PLAY",
	[0x33] = "KEY_REWIND",		/* scan back */
	[0x34] = "KEY_FORWARD",		/* scan forward */
	[0x38] = "KEY_STOP",
	[0x39] = "KEY_PAUSE",
	[0x40] = "KEY_CONTEXT_MENU",	/* pop up/menu */
	[0x43] = "KEY_HOME",		/* top menu */
	[0x50] = "KEY_LIST",		/* Select */
	[0x53] = "KEY_LANGUAGE",	/* Start */
	[0x54] = "KEY_UP",
	[0x55] = "KEY_RIGHT",
	[0x56] = "KEY_DOWN",
	[0x57] = "KEY_LEFT",
	[0x5c] = "KEY_OPTION",		/* triangle */
	[0x5d] = "KEY_BACK",		/* circle */
	[0x5e] = "BTN_0",		/* cross */
	[0x5f] = "KEY_SCREEN",		/* square */
	[0x60] = "KEY_FRAMEBACK",	/* slow/step back */
	[0x61] = "KEY_FRAMEFORWARD",	/* slow/step forward */
	[0x63] = "KEY_SUBTITLE",
	[0x64] = "KEY_AUDIO",
	[0x65] = "KEY_ANGLE",
	[0x70] = "KEY_INFO",		/* display */
	[0x80] = "KEY_BLUE",
	[0x81] = "KEY_RED",
	[0x82] = "KEY_GREEN",
	[0x83] = "KEY_YELLOW",
};

static const uint16_t ps3remote_keymap_remote_buttons[] = {
	[0x00] = KEY_1,
	[0x01] = KEY_2,
	[0x02] = KEY_3,
	[0x03] = KEY_4,
	[0x04] = KEY_5,
	[0x05] = KEY_6,
	[0x06] = KEY_7,
	[0x07] = KEY_8,
	[0x08] = KEY_9,
	[0x09] = KEY_0,
	[0x0b] = KEY_ENTER,		/* center ENTER */
	[0x0e] = KEY_ESC,		/* return */
	[0x0f] = KEY_CLEAR,
	[0x16] = KEY_EJECTCD,
	[0x1a] = KEY_MENU,		/* top menu */
	[0x28] = KEY_TIME,
	[0x30] = KEY_PREVIOUS,
	[0x31] = KEY_NEXT,
	[0x32] = KEY_PLAY,
	[0x33] = KEY_REWIND,		/* scan back */
	[0x34] = KEY_FORWARD,		/* scan forward */
	[0x38] = KEY_STOP,
	[0x39] = KEY_PAUSE,
	[0x40] = KEY_CONTEXT_MENU,	/* pop up/menu */
	[0x43] = KEY_HOME,		/* top menu */
	[0x50] = KEY_LIST,		/* Select */
	[0x53] = KEY_LANGUAGE,		/* Start */
	[0x54] = KEY_UP,
	[0x55] = KEY_RIGHT,
	[0x56] = KEY_DOWN,
	[0x57] = KEY_LEFT,
	[0x5c] = KEY_OPTION,		/* triangle */
	[0x5d] = KEY_BACK,		/* circle */
	[0x5e] = BTN_0,			/* cross */
	[0x5f] = KEY_SCREEN,		/* square */
	[0x60] = KEY_FRAMEBACK,		/* slow/step back */
	[0x61] = KEY_FRAMEFORWARD,	/* slow/step forward */
	[0x63] = KEY_SUBTITLE,
	[0x64] = KEY_AUDIO,
	[0x65] = KEY_ANGLE,
	[0x70] = KEY_INFO,		/* display */
	[0x80] = KEY_BLUE,
	[0x81] = KEY_RED,
	[0x82] = KEY_GREEN,
	[0x83] = KEY_YELLOW,
};


int
verify_id (struct udev_device *dev)
{
	const char *hid_id;
	struct udev_device *parent;
	parent = udev_device_get_parent_with_subsystem_devtype(dev, "hid", NULL);
	hid_id = udev_device_get_property_value(parent, "HID_ID");
	if (strncmp(hid_id, REMOTE_HID_ID, 22)) {
		fprintf(stderr, "open_hidraw: hid_id mismatch (%s)\n", hid_id);
		return 0;
	}
	return 1;
}


/*
 * open_hidraw
 * Returns a FD for reading the hidraw, or -1 if the hidraw did not match.
 */
int
open_hidraw (struct udev_device *dev)
{
	int fd;
	const char *devnode;

	if (!verify_id(dev)) {
		return -1;
	}

	/* Open device for reading */
	devnode = udev_device_get_devnode(dev);
	fd = open(devnode, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		perror("open_hidraw");
		return -1;
	}
	fprintf(stderr, "Opened %s.\n", devnode);
	DEBUG_FN(print_hidinfo(fd));
	return fd;
}


uint16_t
read_hidraw (int fd)
{
	struct report usage;
	memset(&usage, 0, sizeof usage);
	read(fd, &usage, sizeof usage);
	if (usage.key == 0xff) {
		return RELEASE;
	}
	debug_puts(ps3remote_keymap_remote_strings[usage.key]);
	return ps3remote_keymap_remote_buttons[usage.key];
}
