int square2(int a, int b)
{
  int c = a*a+b*b;
  return c;
}
int _start()
{
  // ensure all the function called is in the same section
  int result = square2(3, 4);
  return result;
}

