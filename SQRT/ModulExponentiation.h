#ifndef ModulExponentiation_h
#define ModulExponentiation_h


#include <stdlib.h>


struct listnode {
  int index;
  struct listnode *p;
};


long Modular_Exponentiation(long x,  long y, long p)
{
  long res = 1;      // Initialize result
  
  x = x % p;  // Update x if it is more than or 
  // equal to p
 
  while (y > 0)
    {
      // If y is odd, multiply x with result
      if (y & 1)
	res = (res*x) % p;
 
      // y must be even now
      y = y/2; // y = y/2
      x = (x*x) % p;  
    }
  return res;
}

bool IsPrime(int number)
{

  if (number == 2 || number == 3)
    return true;

  if (number % 2 == 0 || number % 3 == 0)
    return false;

  int divisor = 6;
  while (divisor * divisor - 2 * divisor + 1 <= number)
    {

      if (number % (divisor - 1) == 0)
	return false;

      if (number % (divisor + 1) == 0)
	return false;

      divisor += 6;

    }

  return true;

}

int NextPrime(int a)
{

  while (!IsPrime(++a)) 
    { }
  return a;

}


void listnode_push(listnode **head,listnode **tail,int k){
  if (*head==NULL){
    *head= (listnode *)malloc(sizeof(listnode));
    *tail=*head;
    (*head)->index= k;
    (*head)->p=NULL;
  }else {
    (*tail)->p =  (listnode *)malloc(sizeof(listnode));
    *tail=(*tail)->p;
    (*tail)->index=k;
    (*tail)->p=NULL;
  }
}

int listnode_size(listnode *head,listnode *tail){
  int size=0;
  while (head!=NULL){
    head=head->p;
    size++;
  }
  return size;
}

int element(listnode *head,listnode *tail,int i){
  for(int z=0;z<i;z++)
    head=head->p;
  return head->index;
}



int PrimitiveRoot (int p) {
  //std::vector<int> fact;

  
  listnode *head,*tail;
  head=tail=NULL;
  
  int phi = p-1,  n = phi;
  for (int i=2; i*i<=n; ++i)
    if (n % i == 0) {
      listnode_push(&head,&tail,i);
      
      //fact.push_back (i);
      while (n % i == 0)
	n /= i;
    }
  
  if (n > 1)
    //fact.push_back (n);
    listnode_push(&head,&tail,n);

    
  for (int res=2; res<=p; ++res) {
    bool ok = true;
    for (size_t i=0; i<listnode_size(head,tail)/*fact.size()*/ && ok; ++i)
      ok &= Modular_Exponentiation (res, phi / element(head,tail,i) /*fact[i]*/, p) != 1;
    if (ok && res>p/2 && res<p/2+500)  return res;
  }
  return -1;
}
  

 


 

//} ModExp;



#endif
