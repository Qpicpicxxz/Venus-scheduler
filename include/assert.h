#define assert(x)                                                       \
        if (!(x)) {                                                     \
                printf(RED("\nAssertion failed: %s"), #x); 		\
                printf(BOLD("\nFile: %s Line: %d\n"), __FILE__, __LINE__); \
        printf(BLUE("Please exit the program...\n")); 			\
        while(1){};							\
        }

