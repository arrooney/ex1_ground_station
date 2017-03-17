#include <stdio.h>

int main( int argc, char** argv )
{
  printf("Printing with no args\n");
  printf("Printing with args:\n\targc: %d\n\targv[0]: %s\n", argc, argv[0]);
  printf("getchar(): ");
  char c = getchar();
  printf("got: %c\n", c);
  return 0;
}
    
