#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>


int main(int argc, char *argv[]){
  char txt[255];
  int server, portNumber, pid, n;
  struct sockaddr_in serverAddress;     // server socket address

 if(argc != 3){
    printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
    exit(0);
  }

  if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
     fprintf(stderr, "Socket cannot be created\n");
     exit(1);
  }

  serverAddress.sin_family = AF_INET;
  sscanf(argv[2], "%d", &portNumber);
  serverAddress.sin_port = htons((uint16_t)portNumber);
  if(inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) < 0){
  fprintf(stderr, " inet_pton() has failed\n");
  exit(2);
}

 if(connect(server, (struct sockaddr *) &serverAddress, sizeof(serverAddress))<0){
  fprintf(stderr, "connection has failed, exiting\n");
  exit(3);
 }

  read(server, txt, 255);
  fprintf(stderr, "received txt: %s\n", txt);

  pid=fork();

  if(pid)
     while(1)                         /* reading server's text */
if(n=read(server, txt, 255)){
          txt[n]='\0';
          fprintf(stderr,"%s\n", txt);
          if(!strcasecmp(txt, "GoodBye\n")){
            //  kill(pid, SIGTERM);
             exit(0);
           }
         }

  if(!pid)                           /* sending text to server */
     while(1)
      if(n=read(0, txt, 255)){
         txt[n]='\0';
if(strcmp(txt,"terminate\n")==0)
{
        close(server);
		kill(getppid(), SIGTERM);
        exit(0);

}


         write(server, txt, strlen(txt)+1);
         if(!strcasecmp(txt, "GoodBye\n")){
           // kill(getppid(), SIGTERM);
            exit(0);
          }
      }
}
