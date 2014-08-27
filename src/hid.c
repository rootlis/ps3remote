#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stropts.h>
#include <linux/ioctl.h>
#include <linux/hidraw.h>

#include "hid.h"


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


void
print_hidinfo (int fd)
{
	struct hidraw_report_descriptor rdesc;
	int rdesc_size = 0;

	memset(&rdesc, 0, sizeof rdesc);
	if (ioctl(fd, HIDIOCGRDESCSIZE, &rdesc_size) < 0) {
		perror("HIDIOCGRDESCSIZE");
		return;
	}
	rdesc.size = rdesc_size;
	printf("# rdesc_size = %d\n", rdesc_size);

	if (ioctl(fd, HIDIOCGRDESC, &rdesc) < 0) {
		perror("HIDIOCGRDESC");
		return;
	}
	printf("# rdesc =\n");
	print_rdesc(&rdesc);
	putchar('\n');
	fflush(stdout);
}
