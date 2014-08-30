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
static const char *keymap_strings[] = {
	[0x00] = "KEY_1",		/* number pad	*/
	[0x01] = "KEY_2",
	[0x02] = "KEY_3",
	[0x03] = "KEY_4",
	[0x04] = "KEY_5",
	[0x05] = "KEY_6",
	[0x06] = "KEY_7",
	[0x07] = "KEY_8",
	[0x08] = "KEY_9",
	[0x09] = "KEY_0",

	[0x0b] = "KEY_ENTER",		/* enter	*/
	[0x1a] = "KEY_M",		/* top menu	*/

	[0x30] = "KEY_PREVIOUSSONG",	/* step back	*/
	[0x31] = "KEY_NEXTSONG",	/* step forward */
	[0x32] = "KEY_PLAY",		/* play		*/
	[0x33] = "KEY_REWIND",		/* rewind	*/
	[0x34] = "KEY_FASTFORWARD",	/* fast forward	*/
	[0x38] = "KEY_STOP",		/* stop		*/
	[0x39] = "KEY_PAUSE",		/* pause	*/

	[0x40] = "KEY_P",		/* guide/popup	*/
	[0x43] = "KEY_H",		/* home		*/

	[0x50] = "KEY_L",		/* select/list	*/
	[0x53] = "KEY_A",		/* start/aspect	*/

	[0x54] = "KEY_UP",		/* d-pad up	*/
	[0x55] = "KEY_RIGHT",		/* d-pad right	*/
	[0x56] = "KEY_DOWN",		/* d-pad down	*/
	[0x57] = "KEY_LEFT",		/* d-pad left	*/

	[0x5c] = "KEY_V",		/* triangle	*/
	[0x5d] = "KEY_O",		/* circle	*/
	[0x5e] = "KEY_X",		/* cross	*/
	[0x5f] = "KEY_U",		/* square	*/

	[0x60] = "KEY_PREVIOUSSONG",	/* step back	*/
	[0x61] = "KEY_NEXTSONG",	/* step forward	*/

	[0x63] = "KEY_S",		/* subt		*/
	[0x64] = "KEY_D",		/* audio	*/
	[0x65] = "KEY_N",		/* angle	*/
	[0x70] = "KEY_I",		/* info		*/

	[0x80] = "KEY_B",		/* blue/move	*/
	[0x81] = "KEY_R",		/* red/pip	*/
	[0x82] = "KEY_G",		/* green/swap	*/
	[0x83] = "KEY_Y",		/* yellow	*/
};

static const uint16_t keymap_buttons[] = {
	[0x00] = KEY_1,			/* number pad	*/
	[0x01] = KEY_2,
	[0x02] = KEY_3,
	[0x03] = KEY_4,
	[0x04] = KEY_5,
	[0x05] = KEY_6,
	[0x06] = KEY_7,
	[0x07] = KEY_8,
	[0x08] = KEY_9,
	[0x09] = KEY_0,

	[0x0b] = KEY_ENTER,		/* enter	*/
	[0x1a] = KEY_M,			/* top menu	*/

	[0x30] = KEY_PREVIOUSSONG,	/* step back	*/
	[0x31] = KEY_NEXTSONG,		/* step forward */
	[0x32] = KEY_PLAY,		/* play		*/
	[0x33] = KEY_REWIND,		/* rewind	*/
	[0x34] = KEY_FASTFORWARD,	/* fast forward	*/
	[0x38] = KEY_STOP,		/* stop		*/
	[0x39] = KEY_PAUSE,		/* pause	*/

	[0x40] = KEY_P,			/* guide/popup	*/
	[0x43] = KEY_H,			/* home		*/

	[0x50] = KEY_L,			/* select/list	*/
	[0x53] = KEY_A,			/* start/aspect	*/

	[0x54] = KEY_UP,		/* d-pad up	*/
	[0x55] = KEY_RIGHT,		/* d-pad right	*/
	[0x56] = KEY_DOWN,		/* d-pad down	*/
	[0x57] = KEY_LEFT,		/* d-pad left	*/

	[0x5c] = KEY_V,			/* triangle	*/
	[0x5d] = KEY_O,			/* circle	*/
	[0x5e] = KEY_X,			/* cross	*/
	[0x5f] = KEY_U,			/* square	*/

	[0x60] = KEY_PREVIOUSSONG,	/* step back	*/
	[0x61] = KEY_NEXTSONG,		/* step forward	*/

	[0x63] = KEY_S,			/* subt		*/
	[0x64] = KEY_D,			/* audio	*/
	[0x65] = KEY_N,			/* angle	*/
	[0x70] = KEY_I,			/* info		*/

	[0x80] = KEY_B,			/* blue/move	*/
	[0x81] = KEY_R,			/* red/pip	*/
	[0x82] = KEY_G,			/* green/swap	*/
	[0x83] = KEY_Y,			/* yellow	*/
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


static void
debug_scancode (struct report *usage)
{
	int size;
	int i;
	size = sizeof(struct report);
	printf("<=");
	for (i=size - 2; i>=0; i-=2) {
		printf(" 0x%04x", (*(uint16_t*)((uint8_t*)usage+i)));
	}
	putchar('\n');
}


uint16_t
read_hidraw (int fd)
{
	struct report usage;
	memset(&usage, 0, sizeof usage);
	read(fd, &usage, sizeof usage);
        DEBUG_FN(debug_scancode(&usage));
	if (usage.key == 0xff) {
		return RELEASE;
	}
	debug_print("got 0x%02x = %s\n", usage.key, keymap_strings[usage.key]);
	return keymap_buttons[usage.key];
}
