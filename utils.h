#ifndef UTILS_H
#define UTILS_H

#define DEBUG 0

/* stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing */
#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#define MAX(a,b) ((a)>(b)?(a):(b))

#endif
