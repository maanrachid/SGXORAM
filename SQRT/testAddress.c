#include <stdio.h>
#include <stdlib.h>

struct block{
  int A[16];

};



int main(){
  int a,b;
  char *m=(char*)malloc(40);

  printf("%lld\n%lld\n%lld\n%lld\n",&a,&b,m,m+3);
  int *u= (int*)&m[4];
  u[0]=10;
  u[8]=200000000;
  printf("%d\n",u[8]);

}
