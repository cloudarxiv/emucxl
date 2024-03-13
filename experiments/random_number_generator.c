#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main() {
   srand(1); /* set seed for random number generation */
   int num;
   for (int i = 0; i < 10; i++) {
       num = (rand() % 100) + 1;
       printf("%d\n", num);
   }
   return 0;
}