#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFER 1024


void serviceClient(int);

int main(int argc, char *argv[]){  
  int socketfd, client, portNumber, status;
  struct sockaddr_in serverAddress;      // serv_add contains the add of the server
  
 if(argc != 2){
    printf("Call model: %s <Port Number>\n", argv[0]); //the user should specify the port number on which he should establish connection.
    exit(0);
  }
  if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {  //the socket sys call creates a new socket; takes three agruments;  
							//first is the address of the domain of the socket;
							//second is the typeof the socket;
							//third is the protocol
    fprintf(stderr, "Socket cannot be created\n");
    exit(1);
  }
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  serverAddress.sin_family = AF_INET; //sin_family contains the code of the address family
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); //field contains the IP address of the host
  sscanf(argv[1], "%d", &portNumber);
  serverAddress.sin_port = htons((uint16_t)portNumber); //contains the port number
  
  bind(socketfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
  listen(socketfd, 5);

  while(1){
    printf("Waiting for the client\n");
    client = accept(socketfd, NULL, NULL);
    printf("Client is found\n");
    
    if(!fork())
      serviceClient(client);

    close(client);
    waitpid(0, &status, WNOHANG);
  }
}

void serviceClient(int socketfd){
	
	write(socketfd, "Client is found", 14);
	dup2(socketfd,STDOUT_FILENO);

	while(1){
		char readstr[BUFFER];
		
		printf("virtual-shell/$: ");
		
		
		if(read(socketfd, readstr, BUFFER)<0){
			write(socketfd, "Reading error", 31);
			break;
		}
		//write(socketfd,readstr,16);
		
		
		
	size_t length = strlen(readstr);
		if (readstr[length - 1] == '\n')
			readstr[length - 1] = '\0'; 
		
		if(strcmp(readstr, "exit")==0){      
			 kill(getppid(), SIGTERM);
	         exit(0);
		}

	
	int count=0;
	int commandNo=0;
	char commands[200][100];
	char idn[200];
	int i=0;
	char indentifier=';'; //for unix commands
    while(count <= strlen(readstr)) 
    {		
		if(readstr[count]==';'|| readstr[count]=='&' || readstr[count]=='|')
		{	
			if(readstr[count]=='&')
			{
				indentifier='&';
			}
			else if(readstr[count]=='|')
			{
				indentifier='|';
			}
			else{
				indentifier=';';
			}
			if(readstr[count]=='&' && readstr[count+1]=='&' )
			{
				
			}
			else if(readstr[count]=='|' && readstr[count+1]=='|' )
			{
				
			}
			else{
				commands[commandNo][i]='\0';
				idn[commandNo]=indentifier;			
			    commandNo++;
			 i=0;
			}
		}
		else{
			commands[commandNo][i]=readstr[count];
			i++;
			
		}
		count++;      
    }
	idn[commandNo]=';';
			

for(i=0;i<=commandNo;i++)
{
      printf("\nCommand: %s\n", commands[i]);
      char *args[10] = {}; 
      args[0] = strtok(commands[i], " ");
      int tokenCounter = 0;
      
      while (args[tokenCounter] != NULL){
        tokenCounter++;
        args[tokenCounter] = strtok(NULL, " ");
      }

      int childpid = fork();

      if (childpid == 0){
        if ((execvp(args[0], args)) < 0){
          printf("Error! Command is not recognized.\n");
		  exit(-1);
        }
        exit(0);
      }
	  else if (childpid > 0){  	  
        int status;       
        wait(&status);
        if(WIFEXITED(status))
		{
          int exitstat = WEXITSTATUS(status);
		  if(exitstat ==0)
		  {
          printf("\nChild %d exited with status %d\n", childpid, exitstat);
		   if(idn[i]=='|')
			  {
				  i=commandNo+1;
			  }
		  }		  
			else
			{
			 printf("Error with command");
			  if(idn[i]=='&')
			  {
				  i=commandNo+1;
			  }
			}
        }
		else
		{
			printf("signaled by =%d\n",WTERMSIG(status));
		}
      }else{
        printf("Error\n");
        exit(1);
      }
      
    } 

	}
}