#define DELAY 500
void delay(volatile int count) {
  count *= DELAY;
  while (count--)
    ;
}
