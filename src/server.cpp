#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include "pack109.hpp"

//Headers for the write and read methods
void write(char* filename, vec mySerializedPerson);
vec readNoOpen(FILE* fp);

int main(int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   int a = 0;
   int key = 42;

//Making sure --hostname is present, if not tell user to put --hostname
    for(int i =0; i < argc; i++){
        if (strcmp(argv[i], "--hostname")){
            a = 1;
        } if (a == 0){
            printf("%s", "Error: Must include a --hostname");
            exit(1);
        }
    }
   
// Using strtok to seperate the host location and the portno by use of the delimiter ":"
   char* str = argv[2];
   char* token1 = strtok(str, ":");
   printf("Address %s\n", token1);
   char* token2 = strtok(NULL, "\0");
   printf("Port %s\n", token2);


    //fflush(stdout);


   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));

   //portno = atoi(argv[1]);
   portno = atoi(token2);
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
	
   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }
   
// Created buffer with a size large enough to handle the size of the files that are to be sent
   vec buff(60000);


   /* If connection is established then start communicating */
   bzero((char*) buff.data(),60000);
   printf("Waiting for message \n");
   n = read( newsockfd, (char*)buff.data(), 59999 );
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   } else {
      printf("Read %d bytes. \n", n);
   }
   
   //Decrypting/encrypting

   for(int i = 0; i < n; i++){
      buff[i] = buff[i] ^ key;
      printf("%d ", buff[i]);

   }

   printf("\n");

   if (buff[3] == 4) { //This means its a send file 
      struct File receivedFile = pack109::deserialize_file(buff);

      char str[40];    // str = "    "   
      strcpy (str,"Received/");   // str = "Received/"   
      strcat (str, (receivedFile.name).c_str());   //   if document.txt then // str = "Received/document.txt"   
      printf("Saving the file: \"%s\"\n", str);
      write(str, receivedFile.bytes);


   } else if(buff[3] == 7) { // This means its a request
      struct Request receivedFile = pack109::deserialize_request(buff);
      string fileName = "Received/" + receivedFile.name;
      FILE* fp = NULL;
      fp = fopen( &fileName[0], "r" );
      printf("%p\n", fp);

      if (fp == NULL){
         perror("ERROR. File not found");
         write(newsockfd, "", 0);
         exit(1);
      }
      char readBytes[60000];
      long length = fread( &readBytes[0] , 1 , 60000 , fp );
      fclose(fp);
      printf("%ld number of bytes read\n", length ) ;
      std::vector<unsigned char> readAsVec;
      for (int i = 0; i < length; i++)
        readAsVec.push_back(readBytes[i]);
      struct File fileToSerialize = { receivedFile.name , readAsVec};
      vec serializedFileContents = pack109::serialize(fileToSerialize);   // serializng struct myFile both name of file and contents

      for (int i=0; i<serializedFileContents.size(); i++){    
         serializedFileContents[i] = serializedFileContents[i] ^ key;     
      }
      int n = write(newsockfd, (const char*) &serializedFileContents[0],   serializedFileContents.size());
   }

   printf("Here is the message: %s\n",buffer);
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }   
   return 0;
}

//Used this a reference for the write function: https://www.tutorialspoint.com/c_standard_library/c_function_fwrite.htm

void write(char* filename, vec buffer){
   FILE * fp = fopen( filename, "w" );
   fwrite( &buffer[0] , 1 , buffer.size() , fp );
   fclose(fp);
}
std::vector<unsigned char> readNoOpen(FILE* fp){
   std::vector<unsigned char> buffer;
   fread( &buffer[0], 1 , 60000 , fp );
   fclose(fp);
   return buffer;
}

// For the Base server and client intial connection used this page: https://www.tutorialspoint.com/unix_sockets/socket_quick_guide.htm