#ifndef RDESCRIPTOR_H
#define RDESCRIPTOR_H

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


const char* itemtag_str (unsigned char tag);
const char* itemarg_str (unsigned char tag, unsigned char arg);
void print_rdesc (struct hidraw_report_descriptor *rdesc);

#endif
