#define assert(x)                                                       \
        if (!(x)) {                                                     \
                printf(RED("Assertion failed: %s"), #x); 		\
                printf("  File: %s Line: %d\n", __FILE__, __LINE__); \
        panic("Please exit the program...\n"); 				\
        }

