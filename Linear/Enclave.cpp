#include "Enclave_t.h"

#include "sgx_trts.h"
#include "sgx_tcrypto.h"
#include "ext.h"

#include <string.h>

//#include "wolfssl/wolfcrypt/aes.h"
//#define ARRSIZE 1000
#include "Encryptor.h"



void checkContent(Block message[],int size, char * mess);
void encryptAllValues();
void decryptAllValues();
void do_enclave_work();
void encrypt(Block message[], int size);
//void encrypt_v2(Block message[], int size,int op);
//void encrypt_v3(Block message[], int size,int op);
void printf(const char *fmt, ...);
//void init_Enc();

Block *A=NULL;
sgx_aes_ctr_128bit_key_t key;
InitVectorStr initVector;
Block com_key;
Block clientmessage[3];
int ARRSIZE;
int Mid_Enc_Com,SizeSoFarEnc,SizeSoFarDec,initcounter,currclientmessageindex;
bool enteredfirsttime=false;
bool clientmessagemodified=false;




void initialize(int arrsize, int oram_type){
  
  for(int i=0;i<16;i++)
    key[i]=3;

  

  
  if (A==NULL)
    A = (Block*) malloc(arrsize*sizeof(Block));  

  ARRSIZE = arrsize;
  
  for(int i=0;i<ARRSIZE;i++){
    A[i]=initcounter++;
    //printf("From Enclave: %d\n",(int)A[i]);
  }

  com_key=10000;
  encryptAllValues();
  //decryptAllValues(); // testing

}


void passAllValues(Block *input,size_t len){
  ARRSIZE = len/sizeof(Block);
  for(int i=0;i<ARRSIZE;i++)
    A[i] = input[i];
}


void passMessageFromClient(Block *input,size_t len){
  if (!enteredfirsttime){
    memcpy(clientmessage,input,sizeof(Block)*3);
    /*
    for(int i=0;i<3;i++){
      clientmessage[i]=input[i];
      }*/
	   
  }
  do_enclave_work();
}

void getAllValues(Block *input,size_t len){
  for(int i=0;i<ARRSIZE;i++)
    input[i] = A[i];
}


void getMessageToClient(Block *input,size_t len){
  memcpy(input, clientmessage,sizeof(Block)*3);
  /*
  for(int i=0;i<3;i++){
    input[i]=clientmessage[i];
    }*/
}


void do_enclave_work(){
  //register Block r1;
  //register Block r2;
  

  if (!enteredfirsttime){
    //encrypt(clientmessage,3);  // decrypt message
    //printf("Before Encryption in Enclave:%d\n",(int)clientmessage[1]);
    //encrypt_v3(clientmessage,3,1);
    //printf("After Encryption in Enclave:%d\n",(int)clientmessage[1]);
    //Block mm[3]; memcpy(mm,clientmessage,sizeof(Block)*3);
    //encrypt_v3(mm,3,2);
    //memcpy(mm,clientmessage,sizeof(Block)*3);
    Encrypt(clientmessage,3,2,com_key);
    //printf("After Decryption in Enclave:%d\n",(int)clientmessage[1]);
    //printf("After Decryption in Enclave:%d\n",(int)clientmessage[0]);
    //printf("Before Encryption in Enclave:%d\n",(int)mm[1]);
    //encrypt_v3(mm,3,1);
    //printf("After Encryption in Enclave:%d\n",(int)mm[1]);
    //encrypt_v3(mm,3,2);
    //printf("mm After Decryption in Enclave:%d\n",(int)mm[1]);

    
    
    enteredfirsttime=true;
    currclientmessageindex=(int)clientmessage[1];
    //printf("client message before encr %d\n",  currclientmessageindex);
  }
  
  decryptAllValues();   //decrypt values

  //for(int i=0;i<ARRSIZE;i++) printf("%d\n",(int)A[i]);

  //printf("request from client %d\n",z);
  
  if (currclientmessageindex>=Mid_Enc_Com*ENCLAVESIZE &&
      currclientmessageindex<(Mid_Enc_Com+1)*ENCLAVESIZE &&
      !clientmessagemodified){
    
    if (clientmessage[0]==2){
      A[((int)clientmessage[1])%ENCLAVESIZE]= clientmessage[2];
      clientmessage[0]=4;
    }else if (clientmessage[0]==1){
      clientmessage[0]=3;
      // do the following statement in oblivious way--------  
      clientmessage[2] = A[((int)clientmessage[1])%ENCLAVESIZE];
    
    /*
    for(int i=0;i<ARRSIZE;i++){
      r1 = A[i];
      if (i==clientmessage[1])
	r2=r1;
      A[i]=r1;
    }
    clientmessage[2] = r2;
    */
    // end doing oblivious read
    }
    clientmessagemodified=true;
    //printf("client message directly before encr %d\n",(int)clientmessage[1]);
    //printf("client message directly before encr %d\n",(int)clientmessage[2]);
    //encrypt(clientmessage,3);
    Encrypt(clientmessage,3,1,com_key);
    com_key++; // decrypt then encrypt then change

  }

  Mid_Enc_Com++;
  encryptAllValues();
  //decryptAllValues();  //testing
}


// encrypt all values. passAllValues should be called first
void encryptAllValues(){
  const sgx_aes_ctr_128bit_key_t *p_key= &key;  //key 
  const Block *p_src = A;  // input array
  const int src_len= sizeof(Block); //size of block
  uint8_t *p_ctr=(uint8_t*)&initVector;   // initialization vector
  const int ctr_inc_bits=8;
  Block p_dst[ARRSIZE];


  for(int i=0;i<ARRSIZE;i++){
    InitVectorStr s=initVector;
    s=s+SizeSoFarEnc;
    p_ctr =(uint8_t*) &s;
    sgx_aes_ctr_encrypt(p_key,(uint8_t*)&p_src[i],src_len,p_ctr,ctr_inc_bits,(uint8_t*)&p_dst[i]);
    SizeSoFarEnc++;
  }


  
  for(int i=0;i<ARRSIZE;i++)
   A[i] = p_dst[i];

}

// decrypt all values. passAllValues should be called first
void decryptAllValues(){
  const sgx_aes_ctr_128bit_key_t *p_key= &key;  //key 
  const Block *p_src = A;  // input array
  const int src_len= sizeof(Block); //size of array
  uint8_t *p_ctr=(uint8_t*)&initVector;   // initialization vector
  const int ctr_inc_bits=8;
  Block p_dst[ARRSIZE];


 
  for(int i=0;i<ARRSIZE;i++){
    InitVectorStr s=initVector;
    s= s+SizeSoFarDec;
    //printf("decrypt with %d\n",(int)
    p_ctr = (uint8_t*)&s;
    sgx_aes_ctr_decrypt(p_key,(uint8_t*)&p_src[i],src_len,p_ctr,ctr_inc_bits,(uint8_t*)&p_dst[i]);
    SizeSoFarDec++;
  }

  /*
  if (SizeSoFarEnc==SizeSoFarDec){
    initVector=initVector+SizeSoFarDec;//  initial vector should not be used twice
    SizeSoFarEnc=SizeSoFarDec=0;
    }*/
    
  for(int i=0;i<ARRSIZE;i++)
   A[i] = p_dst[i];

}





void encrypt(Block message[], int size){
  for(int i=0;i<size;i++){
    Block b = (message[i] ^ com_key);
    message[i]=b;
    //message[i]= (message[i] ^ com_to_client_key);
  }
}

void freeMem(){
  free(A);
}

void reSetComCounter(){
  Mid_Enc_Com=0;
  enteredfirsttime=false;
  clientmessagemodified=false;
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



  





