#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<stdlib.h>
#define ENCLAVE_FILE "enclave.signed.so"
//#define ARRSIZE 1000
//#define BUFFERSIZE 10000
#include "ext.h"
#include "MyMath.h"

#include "sgx_urts.h"
#include "Enclave_u.h"



void send_and_receive(int sock,Block* Arr1, Block *Arr2, int size1,int size2);
void send_message_to_server(int sock,Block servermessage[]);
void checkContents(Block *Arr1,char *message,int size);
void choose_initialization(int ARRSIZESQRT,int extra, sgx_enclave_id_t  eid,Block* A);
void choose_reinitialization(int ARRSIZESQRT,int Extra, sgx_enclave_id_t  eid,Block* A);
void show_usage();
bool no_enough_ram(int ARRSIZESQRT,int Extra);
Block * Allocate_T(int Num_of_Cycles,int obliv_shuffle_logN);
void send_T_To_Enclave_OneByOne( sgx_enclave_id_t  eid, int Num_of_Cycles,int obliv_shuffle_logN);


int senttimes=0;
Block *Big_T;
Block *A_ptr;


int main(int argc , char *argv[])
{
  int socket_desc , client_sock , c , read_size, sock;
  struct sockaddr_in middle , client, server;
  Block client_message[3];  // operation , position , newvalue
  Block server_message[3];

  if (argc!=3){
    show_usage();
    exit(0);
  }
  const int ARRSIZE = atoi(argv[2]);
  const int Extra = Sqrt(ARRSIZE);
  
  const int ARRSIZESQRT = ARRSIZE+Extra;

  
  Block* A= new Block[ARRSIZESQRT];
  //Block* B= new Block[ARRSIZESQRT];
  
  A_ptr= A; 
  sgx_enclave_id_t   eid;
  sgx_status_t       ret   = SGX_SUCCESS;
  sgx_launch_token_t token = {0};
  int updated = 0;
  int pacsize = ARRSIZESQRT*sizeof(Block);
  
 
  
  //printf("Buffer Size:%d\n",BUFFERSIZE*sizeof(Block));
  printf("Number of Blocks:%d\n",ARRSIZE);
  printf("Size of a Block: %d\n",sizeof(Block));
  printf("Page Size: %d\n",PAGESIZE);
  printf("P : %f\n",P);

  
  //Create an Enclave
  ret = sgx_create_enclave(ENCLAVE_FILE, SGX_DEBUG_FLAG, &token, &updated, &eid, NULL);


  printf("Creating Enclave.....\n");
  if (ret != SGX_SUCCESS) {
    printf("\nApp: error %#x, failed to create enclave.\n", ret);
  }else
    printf("Enclave is created successfully\n");


  choose_initialization(ARRSIZESQRT, Extra, eid,A);

  

  
  //decryptAllValues(eid,A,ARRSIZESQRT*sizeof(uint8_t));

  /*
  for(int i=0;i<ARRSIZESQRT;i++)    
    printf("%d\n",(int)A[i]);
  */
 
  

  /*
  // connection to Server and get all data ------------------------------------
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
    {
      printf("Could not create socket to connect to server");
    }
  puts("Socket created successfully to connect to Server");
  server.sin_addr.s_addr = inet_addr(argv[1]); // 127.0.0.1
  server.sin_family = AF_INET;
  server.sin_port = htons( atoi(argv[2]) );  // 8889

  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
      perror("connect to server failed. Error");
      return 1;
    }
      
  puts("Connected To Server\n");

  
  //---------------------------------------------------------
  */



  // Send encrypted data to Server , initial value------------------------------
 
  //A[0]=2;   // Server,  Store the data please.
  // sending this data...

  //checkContents(A,"Before Sending...",ARRSIZESQRT);
  

  // ----------------No Server Any more
  //send_and_receive(sock,A,B,pacsize,pacsize);

  
  //checkContents(B,"Server Reply After initialization in Server:",ARRSIZESQRT);
  // end sending to server -------------------------------


  
  //exit(0);
  //Create socket to let Client connect to it -------------------------
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
    {
      printf("Could not create socket");
    }
  puts("Socket created to receive from Client");
     
  //Prepare the sockaddr_in structure
  middle.sin_family = AF_INET;
  middle.sin_addr.s_addr = INADDR_ANY;
  middle.sin_port = htons( atoi(argv[1]) );  //8887

  passAddress(eid,&A,'A',sizeof(Block*));
  
  //Bind
  if( bind(socket_desc,(struct sockaddr *)&middle , sizeof(middle)) < 0)
    {
      //print the error message
      perror("bind failed. Error");
      return 1;
    }
  puts("bind done");
     
  //Listen
  listen(socket_desc , 3);
     
  //Accept and incoming connection
  puts("Waiting for incoming connections from Client...");
  c = sizeof(struct sockaddr_in);
     
  //accept connection from an incoming client
  client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
  if (client_sock < 0)
    {
      perror("accept failed");
      return 1;
    }
  puts("Connection accepted");
  // end connecting to client ----------------------------------------------------------

  int AccessCounter=0;


  
  //Receive a message from client --------------------------------------
  while ( (read_size = recv(client_sock , client_message , 3*sizeof(Block) , 0)) > 0 )
    {
      // This is done all the time. Read all then write all 
      //A[0]=1;   // read all values from server


      
      //send_and_receive(sock,A,B,pacsize,pacsize);
      //checkContents(B,"Server Reply After reading all:");
      //passAllValues(eid,B,pacsize);

      /*
      for(int i=0;i<3;i++){
	printf("%d\n",(int)client_message[i]);
	}*/

      AccessCounter++;
      if (AccessCounter>=Extra){
	// reinitialize
	printf("Reinitialization------\n");
	//server_message[0]=5; // Be ready to read All!! and write All!!
	//send_message_to_server(sock,server_message);
	//A[0]=1;  // Read All
	//send_and_receive(sock,A,B,pacsize,pacsize);
	//reinitialize(eid,B,pacsize);
	choose_reinitialization(ARRSIZESQRT, Extra, eid,A);
	//B = new Block[ARRSIZESQRT];
	AccessCounter=0;
	
	if (!no_enough_ram(ARRSIZESQRT,Extra))
	  getAllValues(eid,A,pacsize);
	// else : A will be filled from enclave


	//checkContents(A,"Server Reply After Re-initialization in Server:",ARRSIZESQRT);
	//A[0]=2;  // write All
	//send_and_receive(sock,A,B,pacsize,pacsize);
      }



      
      passMessageFromClient(eid,client_message,3*sizeof(Block));
      //getAllValues(eid,A,pacsize);
    
      //getMessageToServer(eid,server_message,3*sizeof(Block));
      //for(int i=0;i<3;i++)printf("middle message %d\n",(int)server_message[i]);

      
      //send_message_to_server(sock,server_message);

      
      //passMessageFromServer(eid,server_message,3*sizeof(Block));
     

      //getMessageToServer(eid,server_message,3*sizeof(Block));
      //for(int i=0;i<3;i++)printf("middle message %d\n",(int)server_message[i]);
      //exit(0);
      
      //send_message_to_server(sock,server_message);

     
      
      //passMessageFromServer(eid,server_message,3*sizeof(Block));

      
      getMessageToClient(eid,client_message,3*sizeof(Block));

    
      
      
      
      //checkContents(B,"Server Reply After writing");
     
      write(client_sock,client_message,3 * sizeof(Block));
      //freeMem(eid);
      //exit(0);
    }


  if(read_size == 0)
    {
      puts("Client disconnected");
      fflush(stdout);
    }
  else if(read_size == -1)
    {
      perror("recv failed");
    }
  

  
  close(sock); // disconnect from server

  freeMem(eid);
  //delete [] A;
  // delete [] B;

  
  if(SGX_SUCCESS != sgx_destroy_enclave(eid))
    printf("\nApp: error, failed to destroy enclave.\n");

  
 
     
  return 0;
}




void send_message_to_server(int sock,Block* servermessage){
   if( send(sock , servermessage , 3 * sizeof(Block) , 0) < 0)   // send a  request to server
      {
	puts("Send to server failed");
	exit(0);
      }


     if ( recv(sock , servermessage , 3 * sizeof(Block) , 0) < 0)
      {
	puts("recv failed");
	exit(0);
      }

     for(int i=0;i<3;i++)
       printf("server return %d\n",(int)servermessage[i]);
     
}


void send_and_receive(int sock,Block Arr1[], Block Arr2[], int size1,int size2){

  int counter=0;
  Block* buffer= new Block[BUFFERSIZE];

  
  puts("Sending and Receiving Reply from Server.....\n");

  while (1){

    if (counter+BUFFERSIZE>=size1/sizeof(Block))  // if this is the last packet
      memcpy(buffer,&Arr1[counter], sizeof(Block) * ((size2/sizeof(Block))-counter));
    else
      memcpy(buffer,&Arr1[counter], BUFFERSIZE * sizeof(Block));

    /*
    printf(" buffer before sending --------------------\n");
    for(int i=0;i<10;i++)
      printf("%d\n",(int)buffer[i]);
    */

    

    if( send(sock , buffer , BUFFERSIZE * sizeof(Block) , 0) < 0)   // send a  request to server
      {
	puts("Send to server failed");
	exit(0);
      }


     if ( recv(sock , buffer , BUFFERSIZE * sizeof(Block) , 0) < 0)
      {
	puts("recv failed");
	exit(0);
      }

     //printf("%d\n",senttimes);
  
     
    if (counter+BUFFERSIZE>=size1/sizeof(Block)){    // if this is the last packet
      //printf("receiving the remider %d\n",(size1/sizeof(Block))-counter);
      memcpy(&Arr2[counter], buffer, sizeof(Block) * ((size2/sizeof(Block))-counter));
      break;
    }else
      memcpy(&Arr2[counter],buffer, BUFFERSIZE * sizeof(Block));

    //if (counter+BUFFERSIZE>=size1/sizeof(Block)) break;

    /*
    printf(" buffer comming back --------------------\n");
    for(int i=0;i<10;i++)
      printf("%d\n",Arr2[counter+i]);
    */

    
    counter+= BUFFERSIZE;
  }
	

  delete [] buffer;

}

void checkContents(Block* Arr1,char * message,int size){
  puts(message);
  for(int i=0;i<size;i++)
    //if ((int)Arr1[i]!=0)
    printf("index %d value %d Original Position %d IV %d \n",i,(int)Arr1[i],
	   Arr1[i].getPos(),Arr1[i].getIV());
}


void show_usage(){
  printf("server \[port\]  \[Array size\] \n");
}





/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}

void put_in_BigT_OneByOne(Block *input,unsigned long index, size_t len){
    memcpy(&Big_T[index],input,len);
  //printf("index %d comming from enclave %d \n",index,(int)Big_T[index]);
}



// pass all the array to the enclave if it is small, otherwise pass pieces of size sqrt(n)
void choose_initialization(int ARRSIZESQRT,int Extra, sgx_enclave_id_t  eid,Block* A){
  int v;

  if (no_enough_ram(ARRSIZESQRT,Extra)){
    int obliv_shuffle_logN = P*log(ARRSIZESQRT); // how many square root N do we need for obliv shuffle?
    int Extra_Arrsqrt = Sqrt(ARRSIZESQRT);
    int Num_of_Cycles= ARRSIZESQRT/Extra_Arrsqrt;
    int Extra_with_dummy = Extra_Arrsqrt*obliv_shuffle_logN;
    int Extra_with_dummy_1 =  (Num_of_Cycles+1)*obliv_shuffle_logN;

    
    
    Big_T= Allocate_T(Num_of_Cycles,obliv_shuffle_logN);
    //   dummy_server=true;
    
    //Block* Small_T = (Block*) malloc(sizeof(Block)*Extra_with_dummy_1);

    //printf("Sizes: Big T %d  Extra_Arrsqrt %d \n", (Num_of_Cycles+1)* (Num_of_Cycles+1)*obliv_shuffle_logN, Extra_Arrsqrt);

    passAddress(eid,&Big_T,'T',sizeof(Block*));

    printf("Array will be sent in several pieces.\n");
    // Fill Big T ----------------------
    for(v=0;v<ARRSIZESQRT/(Extra_Arrsqrt);v++){
      //printf("Cycle %d out of %d \n",v,ARRSIZESQRT/Extra_Arrsqrt);
      initialize(eid,Extra_Arrsqrt,Extra,ARRSIZESQRT,false);
      //getAllValuesT(eid,Small_T,Extra_with_dummy_1*sizeof(Block));
      // added one because sqrt N is mapped to sqrt(N)+1 locations
      //checkContents(Small_T,"testing",Extra_with_dummy_1);
      //copy_SmallT_To_BigT(Small_T,Big_T,v,Extra_with_dummy_1,obliv_shuffle_logN);
      //memset(Small_T,0,sizeof(Block)*Extra_with_dummy_1);
     
      //printf("-------------------\n");
      
      //getAllValues(eid,A,Extra_Arrsqrt*sizeof(Block));  
    }
    
    initialize(eid,(ARRSIZESQRT%Extra_Arrsqrt),Extra,ARRSIZESQRT,false);
    printf("Enclave is Initialized. Big T is Filled\n");
    //getAllValues(eid,Small_T,Extra_with_dummy_1*sizeof(Block));
    //copy_SmallT_To_BigT(Small_T,Big_T,v,Extra_with_dummy_1,obliv_shuffle_logN);
    //checkContents(Big_T,"testing",(Num_of_Cycles+1)* (Num_of_Cycles+1)*obliv_shuffle_logN);
    // Big T is filled --------------
    
    //send_All_T_To_Enclave(eid, size_of_bigT,Num_of_Cycles,obliv_shuffle_logN);


    send_T_To_Enclave_OneByOne(eid,Num_of_Cycles,obliv_shuffle_logN);
    
    free(Big_T);
    
    //checkContents(A_ptr,"test",ARRSIZESQRT);
    //exit(0);
    
  } else { // we can send the whole array to the enclave (normal case)
    initialize(eid,ARRSIZESQRT,Extra,ARRSIZESQRT,false);
    printf("Enclave is Initialized. Array is passed in one piece to the enclave\n");
    getAllValues(eid,A,ARRSIZESQRT*sizeof(Block));
  }
  
}


void choose_reinitialization(int ARRSIZESQRT,int Extra, sgx_enclave_id_t  eid,Block* A){
   if (no_enough_ram(ARRSIZESQRT,Extra)){
     printf("Reinitialization will be executed. Array will be sent in several pieces.\n");
     int obliv_shuffle_logN = P*log(ARRSIZESQRT); // how many square root N do we need for obliv shuffle?
     int Extra_Arrsqrt = Sqrt(ARRSIZESQRT);
     int Num_of_Cycles= ARRSIZESQRT/Extra_Arrsqrt;
     int Extra_with_dummy = Extra_Arrsqrt*obliv_shuffle_logN;
     int Extra_with_dummy_1 =  (Num_of_Cycles+1)*obliv_shuffle_logN;

     Big_T= Allocate_T(Num_of_Cycles,obliv_shuffle_logN);
     //dummy_server=true;
     passAddress(eid,&Big_T,'T',sizeof(Block*));
     
     int v=0;
     for(v=0;v<ARRSIZESQRT/(Extra_Arrsqrt);v++){
       reinitialize(eid,&A[v*Extra_Arrsqrt],Extra_Arrsqrt*sizeof(Block));
     }

     if (ARRSIZESQRT%Extra_Arrsqrt>0)
       reinitialize(eid,&A[v*Extra_Arrsqrt],(ARRSIZESQRT%Extra_Arrsqrt)*sizeof(Block));
     printf("Big T is Filled\n");
     
    // Big T is filled --------------
     //send_All_T_To_Enclave(eid, size_of_bigT,Num_of_Cycles,obliv_shuffle_logN);


     send_T_To_Enclave_OneByOne(eid,Num_of_Cycles,obliv_shuffle_logN); 

     free(Big_T);
     
   }else {
     reinitialize(eid,A,ARRSIZESQRT*sizeof(Block));
   }

}

bool no_enough_ram(int ARRSIZESQRT,int Extra){
  return 1;
  int permutation_required_size = (4*ARRSIZESQRT);
  return (  (3*Extra+2*ARRSIZESQRT) * sizeof(Block)+
	   permutation_required_size > ENCLAVESIZE);
}




void copy_T_To_A_Host(Block *input,size_t len,int packet_id,int TotalLen){

  int count = len/sizeof(Block);
  if (packet_id*(count+1)>=TotalLen)
    memcpy(&A_ptr[packet_id*count],input,(count-(packet_id*(count+1)-TotalLen))*sizeof(Block));
  else
    memcpy(&A_ptr[packet_id*count],input,len);


}


Block * Allocate_T(int Num_of_Cycles,int obliv_shuffle_logN){

  Block *temp= (Block*) malloc(sizeof(Block)* (Num_of_Cycles+1)* (Num_of_Cycles+1)*obliv_shuffle_logN);

  /*
  if (temp==NULL){
    printf("Big T cannot be built. Dummy server is on");
    temp = (Block*) malloc(sizeof(Block)* (Num_of_Cycles+1)* (Num_of_Cycles+1)*dummy_log);
    dummy_server=true;
    }*/
  return temp;
}

void send_T_To_Enclave_OneByOne( sgx_enclave_id_t  eid,int Num_of_Cycles,int obliv_shuffle_logN){
  int v;
  int size_of_bigT = (Num_of_Cycles+1)* (Num_of_Cycles+1);
  
  for(v=0;v<size_of_bigT-1;v++){
        send_T_To_Enclave(eid,&Big_T[v*obliv_shuffle_logN],obliv_shuffle_logN*sizeof(Block),Num_of_Cycles+1,false);
  }

  send_T_To_Enclave(eid,&Big_T[v*obliv_shuffle_logN],obliv_shuffle_logN*sizeof(Block),Num_of_Cycles+1,true);
  
}
