#include <stdio.h>
#include "ModulExponentiation.h"


int main(){

  int choice;
  printf("Press 1 to test Next Prime\n");
  printf("Press 2 to test Next Mod Exp \n");
  printf("Press 3 to primitive root \n");
  printf("Press 4 to test list node \n");
  printf("Press 5 to test Is Prime \n"); 
  scanf("%d",&choice);
  if (choice==1)
    printf("%d\n",NextPrime(1000000));
  else if (choice==2){
    printf("Enter base:");
    int base,power,P,N;
    scanf("%d",&base);
    //    printf("Enter Power:");
    //scanf("%d",&power);
    printf("Enter P:");
    scanf("%d",&P);
    printf("Enter Size of Array:");
    scanf("%d",&N);
     for(int power=1;power<N;power++){
      int v = Modular_Exponentiation(base,power,P);
      while (v>=N){
	v = Modular_Exponentiation(base,v,P);
	if (v<0) return 0;
	//printf("%d\n",v);
      }
      printf("%d %d\n",power,v);
     }
    //for(int i=0;i<149;i++)
    //printf("%d\n",Modular_Exponentiation(75,i,149));
  }else if (choice==3){
    printf("%d\n", PrimitiveRoot(149));
  }else if (choice==4){
    listnode *head,*tail;
    head=tail=NULL;
    listnode_push(&head,&tail,10);
    printf("pass first push\n");
    listnode_push(&head,&tail,20);
    printf("pass second push\n");
    listnode_push(&head,&tail,30);
    printf("pass Third push\n");
    printf("size %d\n",listnode_size(head,tail));
    printf("pass size\n");
    printf("element 0 %d \n",element(head,tail,0));
    printf("element 2 %d \n",element(head,tail,2));
  }else if (choice==5)
    printf("is Prime for 100 is %d\n",IsPrime(100));

  
  return 0;

}
