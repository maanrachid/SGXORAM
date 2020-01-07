#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<stdlib.h>
#include <time.h>

#define ENCLAVE_FILE "enclave.signed.so"
//#define ARRSIZE 1000
//#define BUFFERSIZE 10000
#include "ext.h"
#include "MyMath.h"
#include "Tree.h"

#include "sgx_urts.h"
#include "Enclave_u.h"

void send_and_receive(int sock,Block* Arr1, Block *Arr2, int size1,int size2);
void checkContents(Block* Arr1,char *message);
void show_usage();
int senttimes=0;



int main(int argc , char *argv[])
{
  int socket_desc , client_sock , c , read_size, sock;
  struct sockaddr_in middle , client, server;
  Block client_message[3];  // operation , position , newvalue
  //char message[2000];
  //char  server_reply[2000];

  if (argc!=3){
    show_usage();
    exit(0);
  }
  const int ARRSIZE = atoi(argv[2]);

  //Block* A= new Block[ARRSIZE];
  //Block* B= new Block[ARRSIZE];
  sgx_enclave_id_t   eid;
  sgx_status_t       ret   = SGX_SUCCESS;
  sgx_launch_token_t token = {0};
  int updated = 0;
  int pacsize = ARRSIZE*sizeof(Block);
  
 
  
  //printf("Buffer Size:%d\n",BUFFERSIZE*sizeof(Block));
  printf("Number of Blocks:%d\n",ARRSIZE);
  printf("Size of a Block: %d\n",sizeof(Block));
  printf("Page Size: %d\n",PAGESIZE);
  printf("Position map threshold: %d\n",POS_MAP_LIMIT);
  printf("Z: %d\n",Z);
  //Create an Enclave
  ret = sgx_create_enclave(ENCLAVE_FILE, SGX_DEBUG_FLAG, &token, &updated, &eid, NULL);


 
// do something
  
  printf("Creating Enclave.....\n");
  if (ret != SGX_SUCCESS) {
    printf("\nApp: error %#x, failed to create enclave.\n", ret);
  }else
    printf("Enclave is created successfully\n");


  clock_t begin_time = clock();
  printf("Creating Items Binary tree....\n");
  tree_node *tr0 = create_tree(ARRSIZE);
  tree_node_aux **tr_aux=new tree_node_aux*[TREES];
  //traverse(tr0);
  tr0->parent=NULL;
  printf("Creating Items Binary tree is completed.\n");
  printf("Time for creating item trees: %f\n",
	 float( clock () - begin_time ) /  CLOCKS_PER_SEC);


  
  int temp = ARRSIZE;
  int temp1 = find_closet_power_of_two(temp);// find closet size of power of 2
  int trees_count=1;

  if (temp>POS_MAP_LIMIT){
     begin_time =clock();
     temp1/=2;
     do{
       printf("Creating tr%d size:%d...\n",trees_count,temp1);
       tr_aux[trees_count] = create_tree_aux(temp1,trees_count);
       tr_aux[trees_count]->parent=NULL;
       //printf("%d %d\n",tr_aux[trees_count]->right->temp,links_to_leaves_aux[trees_count][0]->temp); 
       temp1/=2;
       trees_count++;
     } while (temp1>POS_MAP_LIMIT);
     printf("Time for Creating auxiliary trees: %f\n",
	    float( clock () - begin_time ) /  CLOCKS_PER_SEC);
  }
 
  passAddress(eid,&tr0,'T',sizeof(tree_node*));
  passAddress(eid,&links_to_leaves,'L',sizeof(tree_node**));
  passAddress(eid,&tr_aux,'X',sizeof(tree_node_aux**));
  passAddress(eid,&links_to_leaves_aux,'A',sizeof(tree_node_aux***));


  begin_time =clock();
  printf("Initialization Process is started...\n");
  initialize(eid,ARRSIZE,trees_count-1);
  printf("Intitialization is completed\n");
  printf("Time for Initialization: %f\n",
	 float( clock () - begin_time ) /  CLOCKS_PER_SEC);

  
  //exit(0);
  //printf("%d\n",links_to_leaves[0]->temp);
  
  int v;
  /*
  for(v=0;v<ARRSIZE/ENCLAVESIZE;v++){
    initialize(eid,ENCLAVESIZE,1);
    //printf("Enclave is Initialized\n");
    getAllValues(eid,&A[v*ENCLAVESIZE],ENCLAVESIZE*sizeof(Block));
  }

  
  initialize(eid,(ARRSIZE%ENCLAVESIZE),1);*/

  /*
  for(int i=0;i<ARRSIZE;i++)
    printf("%d\n",A[i]);
  */
  //exit(0);


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
 
  //  A[0]=2;   // Server,  Store the data please.
  // sending this data...

  //checkContents(A,"Before Sending...");
  

  //send_and_receive(sock,A,B,pacsize,pacsize);
  
  //checkContents(B,"Server Reply After initialization in Server:");
  // end sending to server -------------------------------

   
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


  
  //Receive a message from client --------------------------------------
  while ( (read_size = recv(client_sock , client_message , 3*sizeof(Block) , 0)) > 0 )
    {
     
      passMessageFromClient(eid,client_message,3*sizeof(Block));
     
      //reSetComCounter(eid);
    


      
      getMessageToClient(eid,client_message,3*sizeof(Block));
      
      
      // A[0]=2;

      //send_and_receive(sock,A,B,pacsize,pacsize);
      //checkContents(B,"Server Reply After writing");
     
      write(client_sock,client_message,3 * sizeof(Block));
      //freeMem(eid);
      //exit(0);
    }

  close(sock); // disconnect from server

  freeMem(eid);
  //delete [] A;
  //delete [] B;

  
  if(SGX_SUCCESS != sgx_destroy_enclave(eid))
    printf("\nApp: error, failed to destroy enclave.\n");

  
  if(read_size == 0)
    {
      puts("Client disconnected");
      fflush(stdout);
    }
  else if(read_size == -1)
    {
      perror("recv failed");
    }
     
  return 0;
}





void send_and_receive(int sock,Block* Arr1, Block *Arr2, int size1,int size2){

  int counter=0;
  Block* buffer= new Block[BUFFERSIZE];
  memset(buffer,'\0',BUFFERSIZE*sizeof(Block));
  
  //puts("Sending and Receiving Reply from Server.....\n");

  while (1){

    if (counter+BUFFERSIZE>=size1/sizeof(Block))  // if this is the last packet
      memcpy(buffer,&Arr1[counter], sizeof(Block) * ((size2/sizeof(Block))-counter));
    else
      memcpy(buffer,&Arr1[counter], BUFFERSIZE * sizeof(Block));
    
    senttimes++;
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
   
    counter+= BUFFERSIZE;
  }
	

  delete [] buffer;

}

void checkContents(Block* Arr1,char * message){
  puts(message);
  for(int i=0;i<10;i++)
    printf("%d\n",(int)Arr1[i]);
}


void show_usage(){
  printf("./server \[server port\] \[Array size\]  \n");
  //printf("Oram type: 1 for linear, 2 for sqrt\n");
}


/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}


