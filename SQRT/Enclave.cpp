#include "Enclave_t.h"
#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */
#include "sgx_trts.h"
#include "sgx_tcrypto.h"
#include "ext.h"
#include "ModulExponentiation.h"
#include "MyMath.h"

#include <string.h>
#include "Encryptor.h"



typedef unsigned int uint; 
//#define ARRSIZE 1000



void checkContent(Block message[],int size, char * mess);
void encryptAllValues(Block *A);
void decryptAllValues(Block *A);
void do_enclave_work();
void encrypt(Block message[], int size);
void encryptServerBlock(Block* b);
void decryptServerBlock(Block* b);
void encrypt_T_Block(Block* b ,sgx_aes_ctr_128bit_key_t* Tkey,int size);
void decrypt_T_Block(Block* b ,sgx_aes_ctr_128bit_key_t* Tkey,int size);
void printf(const char *fmt, ...);
void getArrBack(Block b[]);
void permutation(int perm[],int size);
void PseudoRandomPerm(int i);
int PRPerm(int i,int nextprime,int primeroot,int n);
bool Passed_One_Piece();
int Perm(int i);
Block obliv_read_block(Block *Arr,int i,int size);
int obliv_read_int(int *Arr,int i,int size);
int obliv_read_short(unsigned short *Arr,int i,int size);
void obliv_write_block(Block *Arr,int i,int size,Block new_block);
void obliv_write_int(int *Arr,int i,int size,int new_val);
void obliv_write_short(unsigned short *Arr,int i,int size,int new_val);
void obliv_increment_short(unsigned short *Arr,int i,int size,int inc_value);
Block*  MyMallocBlock(int size,char **ptr);
unsigned short*  MyMallocShort(int size,char **ptr);
int *  MyMallocInt(int size,char **ptr);
void MyFree(char *ptr);
void  put_in_BigT_OneByOne_local(Block *T,unsigned long index,size_t size);




Block *A=NULL;int*perm; // for permutation map
Block *T; // for Melbourn Shuffle
unsigned short * mapbucket;
unsigned short* bucketcountersAcc;
unsigned short* bucketcounters; 


int *shelter_index;
Block *shelter_value;
int *shelter_initVector;
sgx_aes_ctr_128bit_key_t key,Tkey;
InitVectorStr initVector;
InitVectorStr TinitVector;
Block com_key;
Block clientmessage[3];
Block servermessage[3];
int ARRSIZE,sqrtarrsize;
int sheltercounter;
bool foundinshelter;
int found_i;
int dummycounter;
bool firstsendtoserver;
int Extra,TotalLen;
long Mid_Enc_Com,initcounter,currclientmessageindex;
bool enteredfirsttime=false;
int initialized= 0;
bool clientmessagemodified=false;
int log_n,sqrt_of_total;
int NextPr, PrimitRoot,OldNextPr,OldPrimitRoot;
int fixbuf;
Block Obliv_Block[2];
int Obliv_int[2];
char *real_shelter_index_ptr,*real_perm_ptr;
char *real_T_ptr,*real_A_ptr,*real_shelter_value_ptr;
char *real_bucketcounters_ptr,*real_bucketcountersAcc_ptr,*real_mapbucket_ptr;
int inc_block,inc_short,inc_int;
Block *T_copy;
Block *A_copy_server;


void reinitialize(Block *input,size_t len){

  if (Passed_One_Piece()){
    free(A);
    // decrypting procedure
    for(int i=0;i<sheltercounter;i++){
      Block b= shelter_value[i];
      int y = shelter_index[i];
      encryptServerBlock(&b);
      obliv_write_block(input,Perm(y),sqrtarrsize,b);

      /* Replaced by oblivious version 
      input[Perm(shelter_index[i])]= shelter_value[i];
      encryptServerBlock(&input[Perm(shelter_index[i])]); 
      */
    }
    
    decryptAllValues(input);
    getArrBack(input);

    memset(perm ,0, ARRSIZE*sizeof(uint));
    memset(shelter_index ,0,sizeof(int)*Extra);
    memset(shelter_value,0,sizeof(int)*Extra);
    
    sheltercounter=0;firstsendtoserver=true;
    dummycounter=ARRSIZE;
  
    permutation(perm,TotalLen);

    input[0]=0;memset(&input[ARRSIZE],0,sizeof(Block)*Extra);


    A = (Block*)malloc(len);
    memcpy(A,input,len);

  
    for(int i=1;i<ARRSIZE;i++){
      obliv_write_block(input,Perm(i),sqrtarrsize,A[i]);

      //replaced by oblivious version 
      //input[Perm(i)]= A[i];     // should be B[perm[i]] = A[i]
    }

 
    memcpy(A,input,len);

    /* 
       for(int i=0;i<sqrtarrsize;i++){
       printf("%3d %3d %3d \n",(int)A[i],(int) perm[i],(int)input[i]);
       }
  
       printf("----------\n");*/
    encryptAllValues(A);
  }else {
    int z= len/sizeof(Block);
    sqrtarrsize = z;
    //printf("initialized %d\n",initialized);
    if (initialized==0){
      OldNextPr=NextPr;
      OldPrimitRoot=PrimitRoot;
      fixbuf=z;
     
    }


    //printf("shelter count%d\n",sheltercounter);
    for(int i=0;i<sheltercounter;i++){
      int perm_of_i = PRPerm(shelter_index[i],OldNextPr,OldPrimitRoot,TotalLen);
      //printf("i:%d perm of i %d\n",shelter_index[i],perm_of_i);
      if (perm_of_i>=fixbuf*initialized && perm_of_i<fixbuf*(initialized+1)){


	//Block b =   shelter_value[i]; b.setPos(shelter_index[i]);
	Block b =   obliv_read_block(shelter_value,i,Extra);
	int g = obliv_read_int(shelter_index,i,Extra);
	b.setPos(g);
	encryptServerBlock(&b);
	obliv_write_block(input,perm_of_i%fixbuf,z,b);
	// replace with oblivious version
	/*
	input[perm_of_i%fixbuf]= shelter_value[i];
	input[perm_of_i%fixbuf].setPos(shelter_index[i]);*/
	//printf("perm %d index %d shelter value %d\n",perm_of_i%z,shelter_index[i],(int)shelter_value[i]);

	//encryptServerBlock(&input[perm_of_i%fixbuf]);
      }
    }


    
    //printf("ARRSIZE TotalLen %d %d\n",ARRSIZE,TotalLen);
    for(int i=0;i<Extra;i++){
      int perm_of_i = PRPerm(ARRSIZE+i,OldNextPr,OldPrimitRoot,TotalLen);
      //printf("dummy i:%d found perm of i %d\n", ARRSIZE+i,perm_of_i);
      if (perm_of_i>=fixbuf*initialized && perm_of_i<fixbuf*(initialized+1)){
	//printf("dummy i:%d found perm of i %d\n", ARRSIZE+i,perm_of_i);

	Block b; b= b+  ARRSIZE+i; b.setPos(ARRSIZE+i);
	encryptServerBlock(&b);
	obliv_write_block(input,perm_of_i%fixbuf,z,b);
	
	//input[perm_of_i%fixbuf]= ARRSIZE+i;
	//input[perm_of_i%fixbuf].setPos(ARRSIZE+i);
	//encryptServerBlock(&input[perm_of_i%fixbuf]);
      }
    }


    
    decryptAllValues(input);
    //A = (Block*)malloc(len);

    memcpy(A,input,len);
    //for(int i=0;i<z;i++) printf("%d %d\n",(int)input[i],input[i].getPos());

    if (initialized==0) A[0]=0;
    initialize(z,Extra,TotalLen,true);

  }

  
}



// get array back before permutation
void getArrBack(Block b[]){
  /*
    int temp = b[1];
    int index=1;
    for(int i=1;i<sqrtarrsize-1;i++){
    b[index] = b[perm[index]];
    index = perm[index];
    }
    b[index]= temp;*/

  Block *z=(Block*)malloc(sizeof(Block)*sqrtarrsize);
  memcpy(z,b,sizeof(Block)*sqrtarrsize);
  
  
  for(int i=1;i<sqrtarrsize;i++){
    //Block b1 = obliv_read_block(z,Perm(i),sqrtarrsize);
    //b[i]=b1;
    
    b[i]= z[Perm(i)]; // replaced with oblivious version
  }
  free(z);
}






void initialize(int arrsize,int Extra1,int totallength,bool reinitial=false){
  

  //long d;
  //d=to_long(Val);
  //printf("Answer %d\n");
    
  sqrtarrsize = arrsize;
  //PseudoRandomPerm(totallength);
  
  
  if (A==NULL && !reinitial){ // done for once
    Extra=Extra1;
    inc_block =  PAGESIZE/sizeof(Block);
    inc_int =  PAGESIZE/sizeof(int);
    inc_short =   PAGESIZE/sizeof(short);
    
    //A = (Block*) malloc(sqrtarrsize*sizeof(Block));
    //A = MyMallocBlock(sqrtarrsize*sizeof(Block));
    // A is initialized next
    //perm = (uint*) malloc(totallength*sizeof(uint));
     
    //shelter_index = (int *) malloc(sizeof(int)*Extra);
    shelter_index = MyMallocInt(sizeof(int)*Extra,&real_shelter_index_ptr);
    
    //shelter_value=  (Block *) malloc(sizeof(Block)*Extra);
    shelter_value = MyMallocBlock(sizeof(Block)*Extra,&real_shelter_value_ptr);
   
    sheltercounter=0;firstsendtoserver=true;
    dummycounter=totallength-Extra;
    ARRSIZE = totallength-Extra;
    TotalLen=totallength;
    if (Passed_One_Piece()){
      perm = MyMallocInt(totallength*sizeof(int),&real_perm_ptr);
      permutation(perm,TotalLen);
    }else{
      PseudoRandomPerm(TotalLen);
      //printf("Next Prime : %d PrimitRoot %d\n",NextPr,PrimitRoot);
    }
  }
 
 

  /*
    for(int i=sqrtarrsize-10;i<sqrtarrsize;i++)
    printf("%d\n",(int)perm[i]);
    printf("----------\n");
  */

  
  if (arrsize==totallength){ // we sent the whole array to enclave
    for(int i=0;i<ARRSIZE;i++){
      Block b;
      b=b+initcounter;
      obliv_write_block(A,Perm(i),sqrtarrsize,b);initcounter++;
      // replaced by oblivious version
      //A[Perm(i)]= initcounter++;     // should be B[perm[i]] = A[i]
    }

    com_key=10000;
    encryptAllValues(A);
    //decryptAllValues(A);
    
  }else {

    if (initialized==0) {
      log_n = P*log(totallength);
      sqrt_of_total = (totallength/Sqrt(totallength))+1;
      
      //T = (Block*) malloc (log_n*sizeof(Block));
      T = MyMallocBlock(log_n*sizeof(Block),&real_T_ptr);

      
      mapbucket = MyMallocShort(sizeof(unsigned short)*sqrtarrsize,&real_mapbucket_ptr);
      bucketcountersAcc =MyMallocShort (sizeof (unsigned short)*sqrt_of_total,&real_bucketcountersAcc_ptr);
      bucketcounters = MyMallocShort (sizeof (unsigned short)*sqrt_of_total,&real_bucketcounters_ptr); 
      
      
      /*
      mapbucket =(unsigned short*) malloc (sizeof (unsigned short)*sqrtarrsize);
      bucketcountersAcc =(unsigned short*) malloc (sizeof (unsigned short)*sqrt_of_total);
      bucketcounters =(unsigned short*) malloc (sizeof (unsigned short)*sqrt_of_total); 
      */

      if (!reinitial){
	//free(A);
	if (sqrt_of_total>sqrtarrsize)
	  A= MyMallocBlock(sizeof(Block)*sqrt_of_total,&real_A_ptr);
	else
	  A= MyMallocBlock(sizeof(Block)*sqrtarrsize,&real_A_ptr);
	//A = (Block *)  realloc (A,sizeof(Block)*sqrt_of_total);
      }
      printf("square root of all %d Log N %d \n",sqrt_of_total,log_n);
     
      
      if (reinitial){
	PseudoRandomPerm(TotalLen);
	//printf("Next Prime : %d PrimitRoot %d\n",NextPr,PrimitRoot);
      }
    }


    
    memset(bucketcountersAcc,0,sqrt_of_total*sizeof(short));
    memset(bucketcounters,0,sqrt_of_total*sizeof(short));
    memset(mapbucket,0,sqrtarrsize*sizeof(short));
    
    
    for(int i=0;i<sqrtarrsize;i++){
      if (!reinitial){
	A[i]=initcounter++;

	//A[i].setPerm(Perm(A[i]));
	A[i].setPos(A[i]);
      }else { // this is re-initialization
	//A[i].setPerm(Perm(A[i].getPos()));	
      }

      obliv_increment_short(bucketcountersAcc,Perm(A[i].getPos())/sqrt_of_total,sqrt_of_total,1);
      obliv_increment_short(bucketcounters,Perm(A[i].getPos())/sqrt_of_total,sqrt_of_total,1);
      
      
      //bucketcountersAcc[Perm(A[i].getPos())/sqrt_of_total]++;
      //bucketcounters[Perm(A[i].getPos())/sqrt_of_total]++;
      
    }

    
    
    for(int i=1;i<sqrt_of_total;i++){ // order is not reserved
      //printf("bucket i %d \n",bucketcountersAcc[i]);
      bucketcountersAcc[i]+=bucketcountersAcc[i-1];
    }

    
    for(int i=0;i<sqrtarrsize;i++){
      int index  = obliv_read_short(bucketcountersAcc,Perm(A[i].getPos())/sqrt_of_total,sqrt_of_total);
      obliv_write_short(mapbucket,index-1,sqrt_of_total,i);
      obliv_increment_short(bucketcountersAcc,Perm(A[i].getPos())/sqrt_of_total,sqrt_of_total,-1);
      
      // replaced with oblivious version
      /*
      mapbucket[bucketcountersAcc[Perm(A[i].getPos())/sqrt_of_total]-1]=i;
      bucketcountersAcc[Perm(A[i].getPos())/sqrt_of_total]--;
      */
    }

  
    int counter=0;int current_loc=0;
    for(int i=0;i<sqrt_of_total;i++){
      memset(T,0,log_n*sizeof(Block));
      for(int j=0;j<bucketcounters[i];j++){
	//	printf("mapbucket[j+current_loc] %d,A %d\n",mapbucket[j+current_loc],
	// (int)A[mapbucket[j+current_loc]]);
	int index = obliv_read_short(mapbucket,j+current_loc,sqrt_of_total);
	Block b = obliv_read_block(A,index,sqrt_of_total);
	T[counter] = b;
	
	/* replaced by oblivious version 
	T[counter]=A[mapbucket[j+current_loc]];
	//T[counter].setPerm(A[mapbucket[j+current_loc]].getPerm());
	T[counter].setPos(A[mapbucket[j+current_loc]].getPos()); */
	
	counter++;
      }
      T[counter].setPos(-1); // dummy start
      current_loc+=bucketcounters[i];
      //printf("counter %d i  %d bucketcounter[i] %d initialize %d \n"
      //   ,counter, i,bucketcounters[i],initialized);
    
      
      if (log_n<=counter){
	 printf("STOP Mebourn Shuffle Fail %d % \n",log_n);
	 for(int p=0;p<counter;p++) printf("%d\n",T[p].getPos());
	 printf("--------------------\n");
	 return;
      }
      
      
      encrypt_T_Block(T,&Tkey,log_n*sizeof(Block));
      unsigned long index = (i*sqrt_of_total+initialized)*log_n;
      put_in_BigT_OneByOne(T,index,log_n*sizeof(Block));
      //printf("pass\n");
      counter=0;
    }
    
 
    if (!reinitial){
      com_key=10000;
    }
    
    initialized++;
  }

  
  /*
    for(int i=sqrtarrsize-100;i<sqrtarrsize;i++){
    printf("original:%d perm:%d A:%d\n",i,(int)perm[i],(int)A[i]);
    }
    printf("----------\n");
  */

  
  /*
    for(int i=0;i<ARRSIZE;i++){
    A[perm[i]]= i;     // should be B[perm[i]] = A[i]
    }*/

    
  
  
  //initVector=initVector+1;
 
  //decryptAllValues(); // testing
  
}



void  put_in_BigT_OneByOne_local(Block *input,unsigned long  index,size_t len){
  memcpy(&T_copy[index],input,len);
}



void passAddress(Block **input,char c,size_t len){
  if (c=='T')
    T_copy = *input;
  else if (c=='A')
    A_copy_server = *input;
}




void send_All_T_To_Enclave(int size_of_bigT,int Num_of_Cycles,int obliv_shuffle_logN){
  int v;
  for(v=0;v<size_of_bigT-1;v++){
    //printf("sendng T %d\n",v);
    send_T_To_Enclave(&T_copy[v*obliv_shuffle_logN],obliv_shuffle_logN*sizeof(Block),Num_of_Cycles+1,false);
  }
  send_T_To_Enclave(&T_copy[v*obliv_shuffle_logN],obliv_shuffle_logN*sizeof(Block),Num_of_Cycles+1,true);
}


int T_items_counter;int packet_id_host;int sentn=0;int countlast=0;
void send_T_To_Enclave(Block *input, size_t len,int cycles,bool isLast){

  
  sentn++;
  decrypt_T_Block(input,&Tkey, len);
  //printf("Decoding : %d\n", (int)input[0]);
  
  
  for(int i=0;i<len/sizeof(Block);i++){
    //int z = input[i].getPerm();
    int pos= input[i].getPos();
    int z= Perm(input[i].getPos());

    
    if (pos==-1)
      break;
    else if (z>=0){
      Block b= obliv_read_block(input,i,cycles);
      b.setPos(pos);
      encryptServerBlock(&b);
      obliv_write_block(A,z%cycles,cycles,b);
      /* replace with oblivious 
      A[z%cycles]=input[i];
      //A[z%cycles].setPerm(z);
      A[z%cycles].setPos(pos);
      encryptServerBlock(&A[z%cycles]); */
      T_items_counter++;
    }
  }
  //printf("%d\n",T_items_counter);
 
 
  
  if (T_items_counter>=cycles || isLast){
    //printf("Num of sent %d %d %d\n",packet_id_host,T_items_counter,cycles);
    // copy A to host  --to be done
    copy_T_To_A_Host(A,cycles*sizeof(Block),packet_id_host,TotalLen);
    memset(A,0,cycles*sizeof(Block));
    //printf("limit reached\n");
    packet_id_host++;
    T_items_counter=0;
    if (isLast) {
      sentn=0;
      packet_id_host=0;
      //free(mapbucket);
      MyFree(real_mapbucket_ptr);
      //free(bucketcountersAcc);
      MyFree(real_bucketcountersAcc_ptr);
      //free(bucketcounters);
      MyFree(real_bucketcounters_ptr);
      //free(T);
      MyFree(real_T_ptr);
      initialized=0;
      memset(shelter_index ,0,sizeof(int)*Extra);
      memset(shelter_value,0,sizeof(int)*Extra);
      
      sheltercounter=0;firstsendtoserver=true;
      dummycounter=ARRSIZE;
      
    }
    
  }
	  

  
}









void passAllValues(Block *input,size_t len){
  for(int i=0;i<ARRSIZE;i++)
    A[i] = input[i];
}



void passMessageFromServer(Block *input,size_t len){
  /*
  for(int i=0;i<3;i++){
    servermessage[i]=input[i];
    }*/

  //printf("passed pos %d\n",(int)servermessage[1]);


    
  decryptServerBlock(&servermessage[2]);

  
  if (clientmessage[0]==1 && foundinshelter){
      //printf("From Enlcave, Read, Found: %d\n",found_i);
      servermessage[0]=2;servermessage[1]= Perm(dummycounter);dummycounter++;
      servermessage[2]= obliv_read_block(shelter_value,found_i,Extra);
      //servermessage[2] = shelter_value[found_i];
      clientmessage[0]=3;
  }else if (clientmessage[0]==1 && !foundinshelter){
      servermessage[0]=2;servermessage[1]= Perm(clientmessage[1]);
      //printf("From Enlcave, Read, NOT Found: %d\n",sheltercounter);
      obliv_write_int(shelter_index,sheltercounter,Extra,(int)clientmessage[1]);
      obliv_write_block(shelter_value,sheltercounter,Extra,servermessage[2]);
      
      //shelter_index[sheltercounter] = clientmessage[1];
      //shelter_value[sheltercounter] = servermessage[2];
      clientmessage[0]=3;
  } else if (clientmessage[0]==2 && foundinshelter){
      servermessage[0]=2;servermessage[1]= Perm(dummycounter);dummycounter++;
      servermessage[2] =clientmessage[2];
      obliv_write_block(shelter_value,found_i,Extra,clientmessage[2]);
      //shelter_value[found_i]=clientmessage[2];
      clientmessage[0]=4;
  } else if (clientmessage[0]==2 && !foundinshelter){
      servermessage[0]=2;servermessage[1]= Perm(clientmessage[1]);
      servermessage[2] =clientmessage[2];
      //printf("From Enlcave, Write, NOT Found: %d\n",sheltercounter);
      
      obliv_write_int(shelter_index,sheltercounter,Extra,(int)clientmessage[1]);
      obliv_write_block(shelter_value,sheltercounter,Extra,clientmessage[2]);

      /*
      shelter_index[sheltercounter] = clientmessage[1];
      shelter_value[sheltercounter] = clientmessage[2];*/
      clientmessage[0]=4;
  }    

  //printf("Requested : %d\n",(int)servermessage[1]);
		 
  if (!foundinshelter)sheltercounter++;
  clientmessage[2]=servermessage[2];
  //encrypt(clientmessage,3);
  Encrypt(clientmessage,3,1,com_key);
  com_key++; // decrypt then encrypt then change
  getMessageToServer(servermessage,3*sizeof(Block));
}


void passMessageFromClient(Block *input,size_t len){
  memcpy(clientmessage,input,3*sizeof(Block));

  /*for(int i=0;i<3;i++){
    clientmessage[i]=input[i];
    }*/
  do_enclave_work();
}

void do_enclave_work(){
  //register Block r1;
  //register Block r2;
  //encrypt(clientmessage,3);  // decrypt message
  Encrypt(clientmessage,3,2,com_key);

  
  foundinshelter=false;
  //servermessage[0]=1;

  int b = Perm(clientmessage[1]); 
  for(int i=0;i<Extra;i++){
    //printf("Shelter, client index:%d %d\n",shelter_index[i],clientmessage[1]);
    if (shelter_index[i]==(int)clientmessage[1]){
      foundinshelter=true;
      Obliv_Block[1]=Perm(dummycounter);
      //servermessage[1]= Perm(dummycounter);
      found_i=i;
      //break;
    }else {
      Obliv_Block[0]=b;
    }
  }

  if (!foundinshelter){
    servermessage[1] = Obliv_Block[0];
    //servermessage[1]=Perm(clientmessage[1]);
  }else {
    servermessage[1]= Obliv_Block[1];
  }

  servermessage[2] = A_copy_server[(int)servermessage[1]];
  passMessageFromServer(servermessage,3*sizeof(Block));
}



void getAllValues(Block *input,size_t len){
  for(int i=0;i<sqrtarrsize;i++)
    input[i] = A[i];
}


void getAllValuesT(Block *input,size_t len){
  for(int i=0;i<len/sizeof(Block);i++)
    input[i] = T[i];
}




void getMessageToClient(Block *input,size_t len){
  memcpy(input,clientmessage,sizeof(Block)*3);
  for(int i=0;i<3;i++){
    input[i]=clientmessage[i];
  }
}

void getMessageToServer(Block *input,size_t len){

  Block *v = &servermessage[2];
  encryptServerBlock(v);
    //printf("Encoding %d with %d\n",(int)servermessage[2],v->getIV());
	   
  A_copy_server[(int)servermessage[1]]=*v;
}







// encrypt all values. passAllValues should be called first
void encryptAllValues(Block* A){
  const sgx_aes_ctr_128bit_key_t *p_key= &key;  //key 
  const Block *p_src = A;  // input array
  const int src_len= sizeof(Block)-4; //size of Block - IV
  uint8_t *p_ctr;//=(uint8_t*)&initVector;   // initialization vector
  const int ctr_inc_bits=8;
  Block p_dst[sqrtarrsize];

  for(int i=0;i<sqrtarrsize;i++){
    InitVectorStr s= initVector;
    p_ctr = (uint8_t*)&s;
    //printf("pos %d encrypt with key %d\n",i,(int)s); 
    sgx_aes_ctr_encrypt(p_key,(uint8_t*)&p_src[i],src_len,p_ctr,ctr_inc_bits,(uint8_t*)&p_dst[i]);
    p_dst[i].setIV((int)initVector);
    initVector= initVector+1;
  }

  
  for(int i=0;i<sqrtarrsize;i++)
    A[i] = p_dst[i];

}

void decryptAllValues(Block *A){
  const sgx_aes_ctr_128bit_key_t *p_key= &key;  //key 
  Block *p_src = A;  // input array
  const int src_len= sizeof(Block)-4; //size of array
  uint8_t *p_ctr;//&initVector;   // initialization vector
  const int ctr_inc_bits=8;
  Block p_dst[sqrtarrsize];

  for(int i=0;i<sqrtarrsize;i++){
    InitVectorStr s;
    int iv = p_src[i].getIV(); 
    s=s+ iv;
    p_ctr = (uint8_t*)&s;
    sgx_aes_ctr_decrypt(p_key,(uint8_t*)&p_src[i],src_len,p_ctr,ctr_inc_bits,(uint8_t*)&p_dst[i]);
  }
  
  for(int i=0;i<sqrtarrsize;i++)
    A[i] = p_dst[i];
}




void encryptServerBlock(Block* b){
  const sgx_aes_ctr_128bit_key_t *p_key= &key;  //key 
  const Block *p_src = b;  // input array
  const int src_len= sizeof(Block)-4; //size of array
  uint8_t *p_ctr;   // initialization vector
  const int ctr_inc_bits=8;
  Block *p_dst=new Block();


  InitVectorStr s= initVector;
  p_ctr = (uint8_t*)&s;
  
  sgx_aes_ctr_encrypt(p_key,(uint8_t*)p_src,src_len,p_ctr,ctr_inc_bits,(uint8_t*)p_dst);
  p_dst->setIV((int)initVector);
  initVector = initVector+1;
  
  *b = *p_dst;
  //printf("encoded with IV %d\n",b->getIV());
  free(p_dst);
}

// decrypt all values. passAllValues should be called first
void decryptServerBlock(Block* b){
  const sgx_aes_ctr_128bit_key_t *p_key= &key;  //key 
  const Block *p_src = b;  // input array
  const int src_len= sizeof(Block)-4; //size of array
  uint8_t *p_ctr;   // initialization vector
  const int ctr_inc_bits=8;
  Block *p_dst=new Block();

  InitVectorStr s;s=s+ b->getIV();
  //printf("Decoding with IV=%d\n",b->getIV());
  p_ctr = (uint8_t*)&s;
  sgx_aes_ctr_decrypt(p_key,(uint8_t*)p_src,src_len,p_ctr,ctr_inc_bits,(uint8_t*)p_dst);
  
  *b = *p_dst;
  free(p_dst);
}

void encrypt_T_Block(Block* b ,sgx_aes_ctr_128bit_key_t* Tkey,int size){
  const sgx_aes_ctr_128bit_key_t *p_key= Tkey;  //key 
  const Block *p_src = b;  // input array
  const int src_len= size-4; //size of array
  uint8_t *p_ctr;   // initialization vector
  const int ctr_inc_bits=8;
  Block p_dst[size/sizeof(Block)];

  InitVectorStr s= TinitVector;
  p_ctr = (uint8_t*)&s;
  
  sgx_aes_ctr_encrypt(p_key,(uint8_t*)p_src,src_len,p_ctr,ctr_inc_bits,(uint8_t*)p_dst);
  //p_dst[size/sizeof(Block)-1].setIV((int)TinitVector);
  p_dst[0].setIV((int)TinitVector);

  
  TinitVector = TinitVector+1;
  
  memcpy(b,p_dst,size);
  
  //free(p_dst);
}

// decrypt all values. passAllValues should be called first
void decrypt_T_Block(Block* b,sgx_aes_ctr_128bit_key_t* Tkey,int size){
  const sgx_aes_ctr_128bit_key_t *p_key= Tkey;  //key 
  Block *p_src = b;  // input array
  const int src_len= size-4; //size of array
  uint8_t *p_ctr;   // initialization vector
  const int ctr_inc_bits=8;
  Block p_dst[size/sizeof(Block)];

  InitVectorStr s;
  //s=s+ b[size/sizeof(Block)-1].getIV();
  s=s+b[0].getIV();
  
  p_ctr = (uint8_t*)&s;
  
  sgx_aes_ctr_decrypt(p_key,(uint8_t*)p_src,src_len,p_ctr,ctr_inc_bits,(uint8_t*)p_dst);
  
  memcpy(b,p_dst,size);
  //free(p_dst);
}








void encrypt(Block message[], int size){
  for(int i=0;i<size;i++){
    Block b = (message[i] ^ com_key);
    message[i]=b;
    //message[i]= (message[i] ^ com_to_client_key);
  }
}


void freeMem(){
  MyFree(real_A_ptr);
  MyFree(real_perm_ptr);
  MyFree(real_shelter_index_ptr);
  //MyFree(shelter_initVector);
  MyFree(real_shelter_value_ptr);
}



/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
void printf(const char *fmt, ...)
{
  char buf[BUFSIZ] = {'\0'};
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, BUFSIZ, fmt, ap);
  va_end(ap);
  ocall_print_string(buf);
}


void  permutation(int perm[],int size){

  uint temp;int index;
  
  perm[0]=0;
  for(uint i=1;i<size;i++){
    perm[i] = i; 
  }

  for(uint i=1;i<size;i++){ // 0 should not be permuted
    uint32_t rand;
    do {
      sgx_read_rand((unsigned char *)&rand,4);
      index = rand%size;uint temp;
    }while (index==0);
    
    temp = perm[i];
    perm[i] = obliv_read_int((int*)perm,index,size);
    //perm[i] = perm[index];

    obliv_write_int((int*)perm,index,size,temp);
    //perm[index] = temp;
  }
}

void PseudoRandomPerm(int i){
  uint32_t rand;
  do {
    sgx_read_rand((unsigned char *)&rand,4);
    int z = rand%1000;
    NextPr = NextPrime(i+z);
    PrimitRoot = PrimitiveRoot(NextPr);
  }while (NextPr==OldNextPr || OldPrimitRoot==PrimitRoot);
}

int PRPerm(int i,int nextprime,int primeroot,int n){
  if (i==0)
    return 0;
  else {
    //printf("%d %d %d \n",primeroot,i,nextprime);
    int u=Modular_Exponentiation(primeroot,i,nextprime);
   
    
    while (u==0 || u>=n)
      u = Modular_Exponentiation(primeroot,u,nextprime);
    return u;
  }
}

int Perm(int i){
  if (Passed_One_Piece()){
    //printf("Error !!!!! %d %d %d \n",sqrtarrsize,TotalLen,sqrtarrsize==TotalLen);
    return perm[i];
  }else
    return PRPerm(i,NextPr,PrimitRoot,TotalLen);
}

bool Passed_One_Piece(){
  return (sqrtarrsize==TotalLen);
}


Block obliv_read_block(Block *Arr,int i,int size){
  /*  
  for(int z=0;z<size;z++){
    if (z==i)
      Obliv_Block[1]= Arr[z];
    else
      Obliv_Block[0]=Arr[z]; // Bluffing
      }
  */
  
  for(int z=0;z<size;z+=inc_block){
    //if (i==z)
    if (i>=z && i<z+inc_block)
      Obliv_Block[1]=Arr[i];
    else
      Obliv_Block[0]=Arr[z];
  }

  return Obliv_Block[1];
}

int obliv_read_int(int *Arr,int i,int size){
  /*
   for(int z=0;z<size;z++){
     if (z==i)
      Obliv_int[1]= Arr[z];
    else
      Obliv_int[0]=Arr[z];
      }*/

 
  for(int z=0;z<size;z+=inc_int){
    //if (i==z)
    if (i>=z && i<z+inc_int)
      Obliv_int[1]=Arr[i];
    else
      Obliv_int[0]=Arr[z];
  }
   
  return Obliv_int[1];
}

int obliv_read_short(unsigned short *Arr,int i,int size){
 
  for(int z=0;z<size;z+=inc_short){
    if (i>=z && i<z+inc_short)
      //if (i==z)
      Obliv_int[1]=Arr[i];
    else
      Obliv_int[0]=Arr[z];
  }
  
  return Obliv_int[1];
}


void obliv_write_block(Block *Arr,int i,int size,Block new_block){
 
  Obliv_Block[1]=new_block;
  for(int z=0;z<size;z+=inc_block){
    Obliv_Block[0]=Arr[z];
    if (i>=z && i<z+inc_block)
      //if (i==z)
      Arr[i]=Obliv_Block[1];
    else
      Arr[z]=Obliv_Block[0];
  }
}



void obliv_write_int(int *Arr,int i,int size,int new_val){
 
  Obliv_int[1]=new_val;
  for(int z=0;z<size;z+=inc_int){
    Obliv_int[0]=Arr[z];
    //if (z==i)
    if (i>=z && i<z+inc_int)
      Arr[i]=Obliv_int[1];
    else
      Arr[z]=Obliv_int[0];
  }
}

void obliv_write_short(unsigned short *Arr,int i,int size,int new_val){

 
  Obliv_int[1]=new_val;
  for(int z=0;z<size;z+=inc_short){
    Obliv_int[0]=Arr[z];
    //if (z==i)
    if (i>=z && i<z+inc_short)
      Arr[i]=Obliv_int[1];
    else
      Arr[z]=Obliv_int[0];
  }
}



void obliv_increment_short(unsigned short *Arr,int i,int size,int inc_value){

  /*
  int l = obliv_read_short(Arr,i,size);
  l+=inc_value;
  obliv_write_short(Arr,i,size,l);
  */
  
  
  for(int z=0;z<size;z+=inc_short){
   
    //if (z==i){
    if (i>=z && i<z+inc_short){
      Obliv_int[0]=Arr[i];
      Arr[i]=Obliv_int[0]+inc_value;
    }else{
      Obliv_int[0]=Arr[z];
      Arr[z]=Obliv_int[0];
    }
  }
}


Block*  MyMallocBlock(int size,char **ptr){

  //return (Block*)malloc(size);
  
  char *Arr;
  Arr = (char*)malloc(size+PAGESIZE);
  *ptr = Arr;

  
  
  if ((long int)Arr%PAGESIZE==0)
    return (Block*) Arr;
  else {
    int i = ((((long int)Arr/PAGESIZE)+1)*PAGESIZE);
    int addr_Arr = (long int) Arr;
    int diff = i-addr_Arr;
    //printf("Waste is %d\n",diff);
    return (Block*)(&Arr[diff]);
  }
}

unsigned short*  MyMallocShort(int size,char **ptr){

  //return (unsigned short*)malloc(size);
  char *Arr;
  Arr = (char*)malloc(size+PAGESIZE);
  *ptr = Arr;

  
  if ((long int)Arr%PAGESIZE==0)
    return (unsigned short*)Arr;
  else{
    int i = ((((long int)Arr/PAGESIZE)+1)*PAGESIZE);
    int addr_Arr = (long int) Arr;
    int diff = i-addr_Arr;
    return (unsigned short*)(&Arr[diff]);
  }
}

int*  MyMallocInt(int size,char **ptr){
  //return (int*)malloc(size);
  char *Arr;
  Arr = (char*)malloc(size+PAGESIZE);
  *ptr=Arr;
 
  
  if ((long int)Arr%PAGESIZE==0)
    return (int *)Arr;
  else{
    int i = ((((long int)Arr/PAGESIZE)+1)*PAGESIZE);
    int addr_Arr = (long int) Arr;
    int diff = i-addr_Arr;
    return (int*)(&Arr[diff]);
  }
  
}

void MyFree(char *ptr){
  free(ptr);
}
