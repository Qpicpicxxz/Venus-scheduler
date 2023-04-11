#include "common.h"

/* actorInit.c */
extern void actor_launch(void);
/* switchContext.c */
void thread_create(void (*task)(void));

void os_main(void) {
   thread_create(actor_launch);
}

