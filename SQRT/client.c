#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <unistd.h>   //close
#include <stdlib.h>
#include <time.h>
#include "ext.h"
#include "Encryptor.h"
//#include "wolfssl/wolfcrypt/aes.h"

//#define ARRSIZE 1000

void encrypt(Block message[], int size);
//void encrypt_v2(Block message[], int size,int op);
//void encrypt_v3(Block message[], int size,int op);
void checkContent(Block message[],int size, char * mess);
void show_usage();
//void init_Enc();

Block com_key;



int main(int argc , char *argv[])
{

  
  int sock;
  struct sockaddr_in middle;
  Block message[3];
  Block server_reply[3];
  if (argc!=5) {show_usage(); exit(0);}
  const int ARRSIZE = atoi(argv[3]);
  int requests = atoi(argv[4]);
  
  com_key=10000;

  if (ENCRYPT_LIB== 2)
    printf("Encryption is done using IPP.\n");
  else if  (ENCRYPT_LIB== 3)
    printf("Encryption is done using wolfssl.\n"); 
  
  
  //Create socket
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
    {
      printf("Could not create socket");
    }
  puts("Socket created. Trying to connect to server");
     
  middle.sin_addr.s_addr = inet_addr(argv[1]); //127.0.0.1
  middle.sin_family = AF_INET;
  middle.sin_port = htons( atoi(argv[2]) ); //8887
  
  
  //Connect to Middle 
  while (connect(sock , (struct sockaddr *)&middle , sizeof(middle)) < 0)
    {
      //perror("connect failed. Error");
      //return 1;
    }
     
  puts("Connected\n");
  srand(time(0));
     
  //keep communicating with Middle
   for(int i=0;i<requests;i++)
     //    while(1)
  
    {

      /*
      do {
	printf("Enter 1 or 2 (read or write) : ");
	scanf("%d" , &message[0]);
      }while (message[0]!=1 && message[0]!=2);
      */
      memset(message,0,sizeof(Block)*3);
      message[0]=1;
 
      

      /*
      do {
	printf("position  : ");
	scanf("%d" , &message[1]);
      } while (message[1]>ARRSIZE-1 || message[1]<1);
      */

      message[1]= rand()%ARRSIZE; if (message[1]==0) message[1]=1;
      message[2]=0;


      
      /*
      if (message[0]==2){
	printf("New value : ");
	scanf("%d" , &message[2]);
	}*/

      //encrypt(message,3);  
      Encrypt(message,3,1,com_key); // encrypt
      
      //encrypt(message,3);  // decrypt for testing
      
      //checkContent(message,3,"After encryption from client:\n"); 

      //encrypt_v3(message,3,2);
      //checkContent(message,3,"After decryption from client:\n");
    
    
      
      
      //Send the message to middle
      printf("Sending request %d\n",i);
      if( send(sock , message , 3*sizeof(Block) , 0) < 0)
        {
	  puts("Send failed");
	  
        }
         
      //Receive a reply from the server
      if( recv(sock , server_reply , 3*sizeof(Block) , 0) < 0)
        {
	  puts("recv failed");
	  break;
        }

      //encrypt(server_reply,3);  // decrypt message
      Encrypt(server_reply,3,2,com_key);
      checkContent(server_reply,3,"Middle Reply:\n");
      

      
      if (server_reply[0]==3) {
	printf("Requested value in position %d is : %d \n",(int)server_reply[1],
	       (int)server_reply[2]);
      } else if (server_reply[0]==4){
	printf("Writing %d in position %d is done \n",(int)server_reply[2],
	       (int)server_reply[1]);
      }

      com_key++;  // encrypt then decrypt then change 
    }
     
  close(sock);
  return 0;
}



void encrypt(Block message[], int size){
  for(int i=0;i<size;i++){
    message[i]= (message[i] ^ com_key);
  }

}



void show_usage(){
  printf("client <middle ip> <middle port> <Array size> <number of requests>  \n");
  
}

void checkContent(Block message[],int size, char * mess){
  printf("%s\n",mess);
  for(int i=0;i<size;i++){
    printf("%d\n",(int)message[i]);
  }

}

  
