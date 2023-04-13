#ifndef __COMMON_H__
#define __COMMON_H__

#include "assert.h"
#include "types.h"

#define DELAY 500

/* printf */
extern int printf(const char* s, ...);  // only understands %d, %x, %p, %s

/* string tools */
extern void delay(volatile int count);

#endif /* __COMMON_H__ */
