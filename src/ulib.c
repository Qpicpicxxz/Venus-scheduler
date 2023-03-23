#include "ulib.h"

/* Function: String copy */
char* strcpy(char* dest, const char* src) {
  char* result;
  result = dest;
  while ((*dest++ = *src++) != 0)
    ;
  return result;
}

/* Funciton: String compare, 0 represent they are the same */
int strcmp(const char* p, const char* q) {
  while (*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

/* Function: String length */
uint32_t strlen(const char* s) {
  int n;
  for (n = 0; s[n]; n++)
    ;
  return n;
}

/* Function: Memory initialization */
void* memset(void* dst, int c, uint32_t n) {
  char* cdst = (char*)dst;
  int i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  return dst;
}

/* Function: String search for match pattern */
char* strchr(const char* s, char c) {
  for (; *s; s++)
    if (*s == c)
      return (char*)s;
  return 0;
}

/* Function: ASCII to integer */
int atoi(const char* s) {
  int n;
  n = 0;
  while ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  return n;
}

void* memmove(void* vdst, const void* vsrc, int n) {
  char* dst;
  const char* src;

  dst = vdst;
  src = vsrc;
  if (src > dst) {
    while (n-- > 0)
      *dst++ = *src++;
  } else {
    dst += n;
    src += n;
    while (n-- > 0)
      *--dst = *--src;
  }
  return vdst;
}

int memcmp(const void* s1, const void* s2, uint32_t n) {
  const char *p1 = s1, *p2 = s2;
  while (n-- > 0) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
    p2++;
  }
  return 0;
}

void* memcpy(void* dst, const void* src, uint32_t n) {
  return memmove(dst, src, n);
}

void memcpy_test(void) {
  void* p = malloc(sizeof(int));
  void* q = malloc(sizeof(int));
  *(uint32_t*)p = 1;
  memcpy(q, p, 4);
  printf("q = %d\n", *(uint32_t*)q);
}

