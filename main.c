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
#define UI_GET_SYSNAME(len)     _IOC(_IOC_READ, UINPUT_IOCTL_BASE, 300, len)

/* PS3 Remote properties */
#define REMOTE_HID_ID "0005:00000609:00000306"
enum remote_info {
	VENDOR_ID	= 0x0609,
	PRODUCT_ID	= 0x0306,
	REPORT_COUNT	= 0x0b,
};


/* Report descriptor stuff */
#define MASK_NUMPARAMS 3

enum tag_types {
	TYPE_MAIN	= 0x0,
	TYPE_GLOBAL	= 0x1,
	TYPE_LOCAL	= 0x2,
	TYPE_RESERVED	= 0x3,
};

enum item_types {
	ITEM_USAGEPAGE = 0x01,
	ITEM_COLLECTION	= 0xa0,
};

struct report {
	uint8_t id;
	uint8_t junk[3];
	uint8_t key;
	uint8_t effs[5];
	uint16_t press;
};

static const char *reserved = "(reserved)";

static const char *main_itemtag_strings[8] = {
	"INPUT",
	"OUTPUT",
	"COLLECTION",
	"FEATURE",

	"END COLLECTION",
	"(reserved)",
	"(reserved)",
	"(reserved)",
};

static const char *global_itemtag_strings[16] = {
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
	"(reserved)",
};

static const char *local_itemtag_strings[16] = {
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
	"(reserved)",
	"(reserved)",
	"(reserved)",
};

static const char *collection_strings[7] = {
	"Physical",
	"Application",
	"Logical",
	"Report",
	"Named Array",
	"Usage Switch",
	"Usage Modifier",
};

/* Keymaps */
enum {
	RELEASE = 0xffff
};
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
	[0x5e] = "BTN_0",			/* cross */
	[0x5f] = "KEY_SCREEN",		/* square */
	[0x60] = "KEY_FRAMEBACK",		/* slow/step back */
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

/* uinput stuff */
enum uinput_keyvals {
	KEYUP	= 0,
	KEYDOWN	= 1,
};



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


const char*
itemtag_str (unsigned char tag)
{
	unsigned char type, prefix;
	type = (tag >> 2) & 0x3;
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


const char*
itemarg_str (unsigned char tag, unsigned char arg)
{
	switch (tag & 0xfa) {
		case ITEM_COLLECTION:
			if (arg > 6) {
				break;
			}
			return collection_strings[arg];
	}
	return NULL;
}


void
print_rdesc (struct hidraw_report_descriptor *rdesc)
{
	int i, num_params;
	unsigned char tag;
	uint32_t arg;
	for (i=0; i<(*rdesc).size;) {
		tag = (*rdesc).value[i++];
		arg = 0;
		num_params = tag & MASK_NUMPARAMS;
		printf("[0x%02x %s]", tag, itemtag_str(tag));
		switch (num_params) {
			case 3:
				arg |= (*rdesc).value[i++];
				arg <<= 8;
				arg |= (*rdesc).value[i++];
				arg <<= 8;
			case 2:
				arg |= (*rdesc).value[i++];
				arg <<= 8;
			case 1:
				arg |= (*rdesc).value[i++];
		}
		printf(" (0x%02x %s)", arg, itemarg_str(tag, arg));
		putchar('\n');
	}
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
*/

	return fd;

open_hidraw_error:
	close(fd);
	return -1;
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


static int
uinput_open (void) 
{
	int fd;
	struct uinput_user_dev uidev;
	if ((fd = open("/dev/uinput", O_WRONLY)) < 0) {
		perror("uinput_open");
		return -1;
	}
	if (ioctl(fd, UI_SET_EVBIT, EV_SYN) < 0) {
		perror("UI_SET_EVBIT");
		goto uinput_open_error;
	}
	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0) {
		perror("UI_SET_EVBIT");
		goto uinput_open_error;
	}
	if (ioctl(fd, UI_SET_KEYBIT, KEY_1) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_2) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_3) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_4) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_5) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_6) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_7) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_8) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_9) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_0) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_ENTER) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_ESC) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_CLEAR) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_EJECTCD) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_MENU) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_TIME) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_PREVIOUS) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_NEXT) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_PLAY) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_REWIND) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_FORWARD) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_STOP) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_PAUSE) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_CONTEXT_MENU) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_HOME) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_LIST) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_LANGUAGE) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_UP) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_RIGHT) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_DOWN) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_LEFT) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_OPTION) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_BACK) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, BTN_0) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_SCREEN) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_FRAMEBACK) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_FRAMEFORWARD) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_SUBTITLE) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_AUDIO) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_ANGLE) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_INFO) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_BLUE) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_RED) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_GREEN) < 0) goto set_keybit_error;
	if (ioctl(fd, UI_SET_KEYBIT, KEY_YELLOW) < 0) goto set_keybit_error;

	memset(&uidev, 0, sizeof uidev);
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "ps3remote");
	uidev.id.bustype = BUS_VIRTUAL;
	uidev.id.vendor  = 0x0609;
	uidev.id.product = 0x0306;
	uidev.id.version = 1;
	if (write(fd, &uidev, sizeof(uidev)) < 0) {
		perror("write");
		goto uinput_open_error;
	}
	if (ioctl(fd, UI_DEV_CREATE) < 0) {
		perror("UI_DEV_CREATE");
		goto uinput_open_error;
	}

	return fd;

set_keybit_error:
	perror("UI_SET_KEYBIT");
uinput_open_error:
	close(fd);
	return -1;
}


static int
uinput_close (int fd)
{
	if (ioctl(fd, UI_DEV_DESTROY) < 0) {
		perror("UI_DEV_DESTROY");
	}
	return close(fd);
}


static int
uinput_write (int fd, uint16_t key, int32_t value)
{
	struct input_event ev[2];
	int i, size;

	memset(&ev, 0, sizeof(ev));
	ev[0].type = EV_KEY;
	ev[0].code = key;
	ev[0].value = value;
	ev[1].type = EV_SYN;
	ev[1].code = SYN_REPORT;

	size = sizeof ev;
/*
	for (i=size/2 - 1; i>=0; i--) {
		printf("0x%02x ", ((uint8_t*)ev)[i]);
	}
	putchar('\n');
	for (i=size - 1; i>=size/2; i--) {
		printf("0x%02x ", ((uint8_t*)ev)[i]);
	}
	putchar('\n');
*/
	write(fd, &(ev[0]), sizeof(ev[0]));
	write(fd, &(ev[1]), sizeof(ev[1]));
	return 0;

	return write(fd, &ev, sizeof(ev));
}


int
main (int argc, char* argv[])
{
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;
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
//		printf("Found existing device\n");
		fd_hidraw = open_hidraw(dev);
		udev_device_unref(dev);
		if (fd_hidraw > -1) {
			break;
		}
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
//			printf("udev event\n");
//			print_dev(dev);
			action = udev_device_get_property_value(dev, "ACTION");
			if (strcmp(action, "add") == 0) {
				fd_hidraw = open_hidraw(dev);
			} else if (strcmp(action, "remove") == 0) {
				close(fd_hidraw);
				fd_hidraw = -1;
			} else {
//				print_dev(dev);
			}
		}
		if (FD_ISSET(fd_hidraw, &fds)) {
			if ((key = read_hidraw(fd_hidraw)) == RELEASE) {
				uinput_write(fd_uinput, oldkey, KEYUP);
				oldkey = -1;
			} else {
				uinput_write(fd_uinput, key, KEYDOWN);
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
