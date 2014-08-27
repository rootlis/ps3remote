#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#define MAX(a,b) ((a)>(b)?(a):(b))
#define REPORT_SIZE 12

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


int
handle_input (int fd)
{
	int i;
	uint8_t buf[REPORT_SIZE];
	memset(buf, 0, sizeof buf);
	read(fd, buf, sizeof buf - 1);
	printf(">");
	for (i=0; i<REPORT_SIZE; i++) {
		printf(" 0x%02x", buf[i]);
	}
	printf("\n");
	return 0;
}


/*
 * handle_hidraw
 * Returns a FD for reading the hidraw, or -1 if the hidraw did not match.
 */
int
handle_hidraw (struct udev_device *dev)
{
	struct udev_device *parent;
	const char *hid_id;
	int fd;

	/* Check if the associated device has the right ID */
	parent = udev_device_get_parent_with_subsystem_devtype(dev, "hid", NULL);
	hid_id = udev_device_get_property_value(parent, "HID_ID");
	if (strncmp(hid_id, "0005:00000609:00000306", 22)) {
		return -1;
	}

	/* Open device for reading */
	fd = open(udev_device_get_devnode(dev), O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		perror("handle_hidraw");
		return -1;
	}

	print_dev(dev);
	return fd;
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
		fd_hidraw = handle_hidraw(dev);
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

		FD_ZERO(&fds);
		FD_SET(fd_udev, &fds);
		FD_SET(fd_hidraw, &fds);
		memset(&tv, 0, sizeof tv);

		ret = select(MAX(fd_udev, fd_hidraw)+1, &fds, NULL, NULL, &tv);
		if (ret < 0) {
			perror("select");
		}
		if (FD_ISSET(fd_udev, &fds)) {
			dev = udev_monitor_receive_device(mon);
			action = udev_device_get_sysattr_value(dev, "Action");
			if (dev && (strncmp(action, "add", 3) == 0)) {
				fd_hidraw = handle_hidraw(dev);
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
