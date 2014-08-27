#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/hidraw.h>
#include "hidraw.h"
#include "remote.h"


/*
 * open_hidraw
 * Returns a FD for reading the hidraw, or -1 if the hidraw did not match.
 */
int
open_hidraw (struct udev_device *dev)
{
	struct udev_device *parent;
	const char *hid_id;
	int fd;
	const char *devnode;

	/* Check if the associated device has the right ID */
	parent = udev_device_get_parent_with_subsystem_devtype(dev, "hid", NULL);
	hid_id = udev_device_get_property_value(parent, "HID_ID");
	if (strncmp(hid_id, REMOTE_HID_ID, 22)) {
		fprintf(stderr, "open_hidraw: hid_id mismatch (%s)\n", hid_id);
		return -1;
	}

//	print_dev(dev);

	/* Open device for reading */
	devnode = udev_device_get_devnode(dev);
	fd = open(devnode, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		perror("open_hidraw");
		return -1;
	}
	fprintf(stderr, "Opened %s.\n", devnode);

/*
	struct hidraw_report_descriptor rdesc;
	int rdesc_size = 0;

	memset(&rdesc, 0, sizeof rdesc);
	if (ioctl(fd, HIDIOCGRDESCSIZE, &rdesc_size) < 0) {
		perror("HIDIOCGRDESCSIZE");
		goto open_hidraw_error;
	}
	rdesc.size = rdesc_size;
	printf("# rdesc_size = %d\n", rdesc_size);

	if (ioctl(fd, HIDIOCGRDESC, &rdesc) < 0) {
		perror("HIDIOCGRDESC");
		goto open_hidraw_error;
	}
	printf("# rdesc =\n");
	print_rdesc(&rdesc);
	putchar('\n');
	fflush(stdout);
	return fd;

open_hidraw_error:
	close(fd);
	return -1;
*/

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
