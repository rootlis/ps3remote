#ifndef UTILS_H
#define UTILS_H

/* stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing */
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#define debug_puts(str) \
            do { if (DEBUG) fprintf(stderr, "%s\n", str); } while (0)
#define DEBUG_FN(fn) \
	do { if (DEBUG) fn; } while (0)

#define MAX(a,b) ((a)>(b)?(a):(b))

#endif
