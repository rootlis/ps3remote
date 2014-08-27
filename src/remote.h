#ifndef REMOTE_H
#define REMOTE_H

#include <linux/input.h>

enum remote_info {
	VENDOR_ID	= 0x0609,
	PRODUCT_ID	= 0x0306,
	REPORT_COUNT	= 0x0b,
};
#define REMOTE_HID_ID 	"0005:00000609:00000306"
#define UREMOTE_NAME 	"VP3700"


/* Keymaps */
enum {
	RELEASE = 0xffff
};

#endif
