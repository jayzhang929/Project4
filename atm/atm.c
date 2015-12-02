#include "atm.h"
#include "ports.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

ATM* atm_create()
{
    ATM *atm = (ATM*) malloc(sizeof(ATM));
    if(atm == NULL)
    {
        perror("Could not allocate ATM");
        exit(1);
    }

    // Set up the network state
    atm->sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&atm->rtr_addr,sizeof(atm->rtr_addr));
    atm->rtr_addr.sin_family = AF_INET;
    atm->rtr_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    atm->rtr_addr.sin_port=htons(ROUTER_PORT);

    bzero(&atm->atm_addr, sizeof(atm->atm_addr));
    atm->atm_addr.sin_family = AF_INET;
    atm->atm_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    atm->atm_addr.sin_port = htons(ATM_PORT);
    bind(atm->sockfd,(struct sockaddr *)&atm->atm_addr,sizeof(atm->atm_addr));

    // Set up the protocol state
    // TODO set up more, as needed
   atm->session = 0;
   memset(atm->username,0x00,251);
    return atm;
}

void atm_free(ATM *atm)
{
    if(atm != NULL)
    {  
	fclose(atm->init);
	atm->session = 0;
	memset(atm->username,0x00,251);
        close(atm->sockfd);
        free(atm);
    }
}

ssize_t atm_send(ATM *atm, char *data, size_t data_len)
{
    // Returns the number of bytes sent; negative on error
    return sendto(atm->sockfd, data, data_len, 0,
                  (struct sockaddr*) &atm->rtr_addr, sizeof(atm->rtr_addr));
}

ssize_t atm_recv(ATM *atm, char *data, size_t max_data_len)
{
    // Returns the number of bytes received; negative on error
    return recvfrom(atm->sockfd, data, max_data_len, 0, NULL, NULL);
}

void atm_process_command(ATM *atm, char *command)
{
    // TODO: Implement the ATM's side of the ATM-bank protocol

	/*
	 * The following is a toy example that simply sends the
	 * user's command to the bank, receives a message from the
	 * bank, and then prints it to stdout.
	 */

	/*
    char recvline[10000];
    int n;

    atm_send(atm, command, strlen(command));
    n = atm_recv(atm,recvline,10000);
    recvline[n]=0;
    fputs(recvline,stdout);
	*/
  	
	char argv1[10000];
	char argv2[10000];
        char message[6];
        char recvline[10];
       atm->sesssion =1;
       message ="Hello";
      atm_send(atm,message,strlen(message));
      n = atm_recv(atm,recvline,10);
      //will do comparison
      recvline[n] = 0;
      fputs(recvline,stdout);

       sscanf(command, "%s %s",argv1,argv2);
        //Check the input command to make sure its valid
	if(strlen(argv1) > 13 || strlen(argv1) <1) {
		printf("Invalid command\n");
		return;	
	}

	if(strcmp(argv1, "begin-session") == 0) {
		//check if there is a user alrea		
		if(atm->session == 1) {
			printf("A user is already logged in\n");
			return;		
		}	
	}

}
