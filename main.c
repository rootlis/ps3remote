#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>

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
	
	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		printf("Can't create udev\n");
		exit(1);
	}
	
	/* Find the remote */
	enumerate = udev_enumerate_new(udev);
	if (udev_enumerate_add_match_subsystem(enumerate, "hid")) {
		perror("udev_enumerate_add_match_subsystem");
		exit(1);
	}
	if (udev_enumerate_add_match_property(enumerate, "HID_ID", "0005:00000609:00000306")) {
		perror("udev_enumerate_add_match_sysattr");
		exit(1);
	}
	if (udev_enumerate_scan_devices(enumerate)) {
		perror("udev_enumerate_scan_devices");
		exit(1);
	}
	devices = udev_enumerate_get_list_entry(enumerate);
	/* For each item enumerated, print out its information.
	   udev_list_entry_foreach is a macro which expands to
	   a loop. The loop will be executed for each member in
	   devices, setting dev_list_entry to a list entry
	   which contains the device's path in /sys. */
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *path;
		
		path = udev_list_entry_get_name(dev_list_entry);
		dev = udev_device_new_from_syspath(udev, path);
		print_dev(dev);
		udev_device_unref(dev);
	}
	/* Free the enumerator object */
	udev_enumerate_unref(enumerate);

	udev_unref(udev);

	return 0;
}
