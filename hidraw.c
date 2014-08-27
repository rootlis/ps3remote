#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/hidraw.h>

#include "hidraw.h"
#include "remote.h"
#include "rdescriptor.h"


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
//	print_hidinfo(fd);
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
//	puts(ps3remote_keymap_remote_strings[usage.key]);
	return ps3remote_keymap_remote_buttons[usage.key];
}
