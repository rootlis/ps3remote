#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
main(int argc, char* argv[])
{
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
	struct udev_device *dev;
	struct udev_monitor *mon;
	int fd;
	
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
		exit(1);
	}
	if (udev_enumerate_scan_devices(enumerate)) {
		perror("udev_enumerate_scan_devices");
		exit(1);
	}
	devices = udev_enumerate_get_list_entry(enumerate);
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path, *hid_id;
		struct udev_device *parent;
		path = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(udev, path);

		/* Check if the associated device has the right ID */
		parent = udev_device_get_parent_with_subsystem_devtype(dev, "hid", NULL);
		hid_id = udev_device_get_property_value(parent, "HID_ID");
		if (strncmp(hid_id, "0005:00000609:00000306", 22)) {
			continue;
		}

		print_dev(dev);
		udev_device_unref(dev);
	}
	udev_enumerate_unref(enumerate);

	/* Monitor for hidraw devices */
	mon = udev_monitor_new_from_netlink(udev, "udev");
	if (udev_monitor_filter_add_match_subsystem_devtype(mon, "hidraw", NULL)) {
		perror("udev_monitor_filter_add_match_subsystem_devtype");
		exit(1);
	}
	udev_monitor_enable_receiving(mon);
	fd = udev_monitor_get_fd(mon);
	while(1) {
		fd_set fds;
		struct timeval tv;
		int ret;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		memset(&tv, 0, sizeof tv);

		ret = select(fd+1, &fds, NULL, NULL, &tv);
		if (ret > 0 && FD_ISSET(fd, &fds)) {
			if ((dev = udev_monitor_receive_device(mon))) {
				print_dev(dev);
			}
		} else if (ret < 0) {
			perror("select");
			exit(1);
		}
		usleep(250*1000);
		printf(".");
		fflush(stdout);
	}
	udev_monitor_unref(mon);

	udev_unref(udev);

	return 0;
}
