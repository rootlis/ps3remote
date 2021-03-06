#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hidraw.h"
#include "uinput.h"
#include "remote.h"
#include "utils.h"

int
find_hidraw (struct udev *udev)
{
	int fd = -1;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_device *dev;
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
		debug_puts("Found existing device");
		fd = open_hidraw(dev);
		udev_device_unref(dev);
		if (fd > -1) {
			break;
		}
	}
	udev_enumerate_unref(enumerate);
	return fd;
}


int
main (int argc, char* argv[])
{
	struct udev *udev;
	struct udev_device *dev;
	struct udev_monitor *mon;
	int fd_udev, fd_hidraw, fd_uinput;
	uint16_t key, oldkey = -1;

	fd_uinput = uinput_open();

	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		fprintf(stderr, "Can't create udev\n");
		exit(1);
	}

	fd_hidraw = find_hidraw(udev);

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
		const char *action;

		memset(&tv, 0, sizeof tv);
		tv.tv_sec = 5;
		FD_ZERO(&fds);
		FD_SET(fd_udev, &fds);
		if (fd_hidraw > 0) {
			FD_SET(fd_hidraw, &fds);
		}

		if (select(MAX(fd_udev, fd_hidraw)+1, &fds, NULL, NULL, &tv) < 0) {
			perror("select");
			break;
		}
		if (FD_ISSET(fd_udev, &fds)) {
			dev = udev_monitor_receive_device(mon);
			debug_puts("udev event");
//			DEBUG_FN(print_dev(dev));
			action = udev_device_get_property_value(dev, "ACTION");
			if (strcmp(action, "add") == 0) {
				fd_hidraw = open_hidraw(dev);
			} else if (strcmp(action, "remove") == 0) {
				close(fd_hidraw);
				fd_hidraw = -1;
			} else {
//				DEBUG_FN(print_dev(dev));
			}
		}
		if (FD_ISSET(fd_hidraw, &fds)) {
			if ((key = read_hidraw(fd_hidraw)) == RELEASE) {
				uinput_sendkey(fd_uinput, oldkey, KEYUP);
				oldkey = -1;
			} else {
				uinput_sendkey(fd_uinput, key, KEYDOWN);
				oldkey = key;
			}
		}
	}
	if (fd_hidraw != -1) {
		close(fd_hidraw);
	}
	udev_monitor_unref(mon);
	udev_unref(udev);

	uinput_close(fd_uinput);

	return 0;
}
