
#ifndef ext_h
#define ext_h

#include <string.h>
#include <stdio.h>
#define BUFFERSIZE 1000
#define ELEMENTNUM 4
#define ENCLAVESIZE 64000000
#define PAGESIZE 4096
#define P 1.0
#define ENCRYPT_LIB 2
//typedef long long Block;


typedef struct Bloc {

  Bloc(){memset(this,0,sizeof(struct Bloc));}
  struct Bloc& operator= (const struct Bloc& B){for(int i=0;i<ELEMENTNUM;i++) this->element[i] = B.element[i];return *this;}
    struct Bloc& operator^ (const struct Bloc& B){ int v = this-> element[0]; this->element[0] = v ^ B.element[0];return *this;}
      struct Bloc& operator= (const int i){this->element[0] = i;return *this;}
    operator int(){ return this->element[0];}
    struct Bloc& operator++ (int i){ this->element[0]+=1;return *this;}
  bool operator== (const int i){return this->element[0]==i;}
  int getPos(){return this ->element[1];}
  void setPos(int i){this->element[1]=i;}
    int getIV(){int i; i=this->element[ELEMENTNUM-1];return i;}
  void setIV(int i){this->element[ELEMENTNUM-1]=i;}
  
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
  int element[16];
}InitVectorStr;


#endif
