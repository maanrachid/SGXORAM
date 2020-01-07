
#ifndef ext_h
#define ext_h

#include <string.h>
#include <stdio.h>


#define BUFFERSIZE 500
#define ELEMENTNUM 4
#define ENCLAVESIZE 100000
#define ENCRYPT_LIB 2 // 1 silly xor , 2 IPP AES_CBC, 3 Wolfssl AES_CBC  


//typedef long long Block;


typedef struct Bloc {

  Bloc(){memset(this,0,sizeof(struct Bloc));}
  struct Bloc& operator= (const struct Bloc& B){ this->element[0] = B.element[0];return *this;}
    struct Bloc& operator^ (const struct Bloc& B){ int v = this-> element[0]; this->element[0] = v ^ B.element[0];return *this;}
  struct Bloc& operator= (const int i){this->element[0] = i; return *this;}
    operator int(){ return this->element[0];}
  struct Bloc& operator++ (int i){ this->element[0]+=1;return *this;}
  bool operator== (const int i){return this->element[0]==i;}
  bool operator<= (const int i){return this->element[0]<=i;}

  int element[ELEMENTNUM];

  
} Block;


typedef struct InitVectorStr{

  InitVectorStr(){memset(this,0,sizeof(struct InitVectorStr));}
  struct InitVectorStr& operator= (const struct InitVectorStr& B){ this->element[0] = B.element[0];return *this;}
    struct InitVectorStr& operator= (const int i){this->element[0] = i;return *this;}
      operator int(){ return this->element[0];}
  struct InitVectorStr& operator++ (int i){ this->element[0]+=1;return *this;}
  struct InitVectorStr& operator+(int i){this->element[0]+=i;return *this;} 
  bool operator== (const int i){return this->element[0]==i;}
  int element[4];
}InitVectorStr;




#endif
