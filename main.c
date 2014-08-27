#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stropts.h>
#include <linux/hidraw.h>
#include <linux/uinput.h>

#define MAX(a,b) ((a)>(b)?(a):(b))


void
print_dev (struct udev_device *dev)
{
	struct udev_list_entry *property, *properties;
	printf("%s\n", udev_device_get_syspath(dev));
	properties = udev_device_get_properties_list_entry(dev);
	udev_list_entry_foreach(property, properties) {
		printf("  %s: %s\n",
			udev_list_entry_get_name(property),
			udev_list_entry_get_value(property));
		udev_list_entry_get_next(property);
	}
	properties = udev_device_get_sysattr_list_entry(dev);
	udev_list_entry_foreach(property, properties) {
		printf("  %s: %s\n",
			udev_list_entry_get_name(property),
			udev_list_entry_get_value(property));
		udev_list_entry_get_next(property);
	}
}


void
print_devtree (struct udev_device *dev)
{
	for (; dev; dev=udev_device_get_parent(dev)) {
		print_dev(dev);
	}
}


#define MASK_NUMPARAMS 3
const char *main_itemtag_strings[8] = {
	"INPUT",
	"OUTPUT",
	"COLLECTION",
	"FEATURE",

	"END COLLECTION",
	"(reserved)",
	"(reserved)",
	"(reserved)"
};
const char *global_itemtag_strings[16] = {
	"USAGE PAGE",
	"LOGICAL MINIMUM",
	"LOGICAL MAXIMUM",
	"PHYSICAL MINIMUM",

	"PHYSICAL MAXIMUM",
	"UNIT EXPONENT",
	"UNIT",
	"REPORT SIZE",

	"REPORT ID",
	"REPORT COUNT",
	"PUSH",
	"POP",

	"(reserved)",
	"(reserved)",
	"(reserved)",
	"(reserved)"
};
const char *local_itemtag_strings[16] = {
	"USAGE",
	"USAGE MINIMUM",
	"USAGE MAXIMUM",
	"DESIGNATOR INDEX",

	"DESIGNATOR MINIMUM",
	"DESIGNATOR MAXIMUM",
	"STRING INDEX",
	"STRING MINIMUM",

	"STRING MAXIMUM",
	"DELIMITER",
	"(reserved)",
	"(reserved)",

	"(reserved)",
	"(reserved)"
	"(reserved)"
	"(reserved)"
};
const char *reserved = "(reserved)";


enum tag_types {
	TYPE_MAIN,
	TYPE_GLOBAL,
	TYPE_LOCAL,
	TYPE_RESERVED
};


const char*
itemtag_str (unsigned char tag)
{
	unsigned char type, prefix;
	type = (tag >> 2) & 0x2;
	switch (type) {
		case TYPE_MAIN:
			prefix = (tag >> 4) & 0x7;
			return main_itemtag_strings[prefix];
		case TYPE_GLOBAL:
			prefix = (tag >> 4) & 0xf;
			return global_itemtag_strings[prefix];
		case TYPE_LOCAL:
			prefix = (tag >> 4) & 0xf;
			return local_itemtag_strings[prefix];
	}
	return reserved;
}


void
print_rdesc (struct hidraw_report_descriptor *rdesc)
{
	int i, num_params;
	unsigned char tag;
	for (i=0; i<(*rdesc).size;) {
		tag = (*rdesc).value[i++];
		num_params = tag & MASK_NUMPARAMS;
		printf("[0x%02x]", tag);
		switch (num_params) {
			case 3:
				printf(" 0x%02x", (*rdesc).value[i++]);
				printf(" 0x%02x", (*rdesc).value[i++]);
			case 2:
				printf(" 0x%02x", (*rdesc).value[i++]);
			case 1:
				printf(" 0x%02x", (*rdesc).value[i++]);
		}
		printf(" \t%s\n", itemtag_str(tag));
	}
}


int
handle_input (int fd)
{
	int i;
	uint8_t buf[12];
	memset(buf, 0, sizeof buf);
	read(fd, buf, sizeof buf - 1);
	printf(">");
	for (i=0; i<12; i++) {
		printf(" 0x%02x", buf[i]);
	}
	printf("\n");
	return 0;
}


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

	struct hidraw_report_descriptor rdesc;
	const char *devnode;
	int rdesc_size = 0;

	/* Check if the associated device has the right ID */
	parent = udev_device_get_parent_with_subsystem_devtype(dev, "hid", NULL);
	hid_id = udev_device_get_property_value(parent, "HID_ID");
	if (strncmp(hid_id, "0005:00000609:00000306", 22)) {
		fprintf(stderr, "open_hidraw: hid_id mismatch (%s)\n", hid_id);
		return -1;
	}

	print_dev(dev);

	/* Open device for reading */
	devnode = udev_device_get_devnode(dev);
	fd = open(devnode, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		perror("open");
		return -1;
	}
	printf("# Opened %s.\n", devnode);

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
}


int
main(int argc, char* argv[])
{
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_device *dev;
	struct udev_monitor *mon;
	int fd_udev, fd_hidraw;
	
	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		printf("Can't create udev\n");
		exit(1);
	}
	
	/* Find the hidraw device */
	enumerate = udev_enumerate_new(udev);
	if (udev_enumerate_add_match_subsystem(enumerate, "hidraw")) {
		perror("udev_enumerate_add_match_subsystem");
	}
	if (udev_enumerate_scan_devices(enumerate)) {
		perror("udev_enumerate_scan_devices");
	}
	devices = udev_enumerate_get_list_entry(enumerate);
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path;
		path = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(udev, path);
		printf("Found existing device\n");
		fd_hidraw = open_hidraw(dev);
		udev_device_unref(dev);
	}
	udev_enumerate_unref(enumerate);

	/* Monitor for hidraw devices */
	mon = udev_monitor_new_from_netlink(udev, "udev");
	if (udev_monitor_filter_add_match_subsystem_devtype(mon, "hidraw", NULL)) {
		perror("udev_monitor_filter_add_match_subsystem_devtype");
	}
	udev_monitor_enable_receiving(mon);
	fd_udev = udev_monitor_get_fd(mon);
	while(1) {
		fd_set fds;
		struct timeval tv;
		int ret;
		const char *action;

		memset(&tv, 0, sizeof tv);
		FD_ZERO(&fds);
		FD_SET(fd_udev, &fds);
		if (fd_hidraw > 0) {
			FD_SET(fd_hidraw, &fds);
		}

		ret = select(MAX(fd_udev, fd_hidraw)+1, &fds, NULL, NULL, &tv);
		if (ret < 0) {
			perror("select");
		}
		if (FD_ISSET(fd_udev, &fds)) {
			dev = udev_monitor_receive_device(mon);
			printf("udev event\n");
			print_dev(dev);
			action = udev_device_get_property_value(dev, "ACTION");
			if (strcmp(action, "add") == 0) {
				fd_hidraw = open_hidraw(dev);
			} else if (strcmp(action, "remove") == 0) {
				close(fd_hidraw);
				fd_hidraw = -1;
			} else {
				print_dev(dev);
			}
		}
		if (FD_ISSET(fd_hidraw, &fds)) {
			handle_input(fd_hidraw);
		}
		usleep(250*1000);
	}
	udev_monitor_unref(mon);

	udev_unref(udev);

	return 0;
}
