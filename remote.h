#ifndef REMOTE_H
#define REMOTE_H

#include <linux/input.h>

/* PS3 Remote properties */
#define REMOTE_HID_ID "0005:00000609:00000306"
enum remote_info {
	VENDOR_ID	= 0x0609,
	PRODUCT_ID	= 0x0306,
	REPORT_COUNT	= 0x0b,
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

#endif
