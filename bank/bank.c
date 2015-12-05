#include "bank.h"
#include "ports.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "reg_check.c"

Bank* bank_create()
{
    Bank *bank = (Bank*) malloc(sizeof(Bank));
    if(bank == NULL)
    {
        perror("Could not allocate Bank");
        exit(1);
    }

    // Set up the network state
    bank->sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&bank->rtr_addr,sizeof(bank->rtr_addr));
    bank->rtr_addr.sin_family = AF_INET;
    bank->rtr_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    bank->rtr_addr.sin_port=htons(ROUTER_PORT);

    bzero(&bank->bank_addr, sizeof(bank->bank_addr));
    bank->bank_addr.sin_family = AF_INET;
    bank->bank_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    bank->bank_addr.sin_port = htons(BANK_PORT);
    bind(bank->sockfd,(struct sockaddr *)&bank->bank_addr,sizeof(bank->bank_addr));

    // Set up the protocol state
    // TODO set up more, as needed
    bank->users = list_create();

    return bank;
}

void bank_free(Bank *bank)
{
    if(bank != NULL)
    {
        close(bank->sockfd);
        free(bank);
    }
}

ssize_t bank_send(Bank *bank, char *data, size_t data_len)
{
    // Returns the number of bytes sent; negative on error
    return sendto(bank->sockfd, data, data_len, 0,
                  (struct sockaddr*) &bank->rtr_addr, sizeof(bank->rtr_addr));
}

ssize_t bank_recv(Bank *bank, char *data, size_t max_data_len)
{
    // Returns the number of bytes received; negative on error
    return recvfrom(bank->sockfd, data, max_data_len, 0, NULL, NULL);
}

void bank_process_local_command(Bank *bank, char *command, size_t len)
{
    static char create_user[] = "create-user";
    static char deposit[] = "deposit";
    static char balance[] = "balance";
    static char invalid[] = "Invalid command";
    static char create_user_error[] = "Usage: create-user <user-name> <pin> <balance>";
    static char error_file[] = "Error creating card file for user";
    static char balance_error[] = "Usage: balance <user-name>";
    static char deposit_error[] = "Usage: deposit <user-name> <amt>";

    const char space[] = " ";
    char user_name[1024];
    char file_name[1024];
    char *token;
    strtok(command, "\n");
    token = strtok(command, space);

    /*
    while( token != NULL ) {
        printf( "%s\n", token );
    
        token = strtok(NULL, space);
    }
    */

    if (strcmp(token, create_user) == 0) {
        char pin[1024];
 	    char balbuf[1024];
        unsigned int balance;

        // get user name
        token = strtok(NULL, space);

        
        if (token != NULL && user_name_check(token)) {
            
            strcpy(user_name, token);
            
            // get user pin number
            token = strtok(NULL, space);

            
            if (token != NULL && pin_check(token)) {
                strcpy(pin, token);
                // get user balance
                token = strtok(NULL, space);
                
                if (token != NULL) {
                    char *ptr;
                    balance = strtoul(token, &ptr, 10);
                    // printf("balance is: %d\n", balance);
                    token = strtok(NULL, space);

                    if (token != NULL) {
                        printf("%s\n", create_user_error);
                    } else {
                    
                        char *key;
                        key = (char *) malloc(sizeof(user_name) * sizeof(char));

                        strcpy(key, user_name);
                        if (list_find(bank->users, key) == NULL) {

                            unsigned int *balance_pt;
                            balance_pt = (unsigned int *) malloc(sizeof(unsigned int));
                            *balance_pt = balance;

                            list_add(bank->users, key, balance_pt);
                            
                            // create card file name
                            strcpy(file_name, user_name);
                            strcat(file_name, ".card");
                            
                            // write pin number and balance (with an empty space in between) into the file
			                char call[1024];
                            //get pin and balance in one string i.e
			                // sprintf(balbuf,"%d",balance);
			                // strcat(pin," ");
		                    // strcat(pin,balbuf);  
                            // printf("%s\n",pin); //pin = "1234 100"
			
		                    //To encrypt and make system call to create .card file
			                sprintf(call,"echo %s |openssl enc -aes-256-cbc -e -a -pass file:%s -salt -out %s",pin,bank->symm_key,file_name);
			                system(call);
			
			                //flush call to get 
			                memset(call,0x00,strlen(call));
                               
                            //TO decrypt
                            /*
				            FILE *fp, *dec;
				            char decrypted[1024];
				            char cipher[1024];


				            //open .card file and read in cipher
				            dec = fopen(file_name, "r");
				            fgets(cipher, 1024, (FILE*)dec);			
				            strtok(cipher, "\n");
				            fclose(dec);
                            
                            //make call command
				            sprintf(call,"echo %s |openssl enc -aes-256-cbc -d -a -pass file:%s -salt",cipher,bank->symm_key);

                            //Get command from console
				            fp =popen(call,"r");
                                 
                            //if all goes well
				            if(fp!=NULL) {
				                fgets(decrypted,1024,fp);
				                //Can now use decrypted text for whatever
				                printf("pin and amount are: %s",decrypted); 

				            }else {
				                printf("Error couldn't open .card file");
				            }
				            
                            pclose(fp);
                            */
                        
                        } else {
                            free(key);
                            printf("%s ", "Error: user");
                            printf("%s ", user_name);
                            printf("%s\n", "already exists");
                        }

                        // list_print(bank->users);
                    }
                } else 
                    printf("%s\n", create_user_error);
            } else 
                printf("%s\n", create_user_error); 
            

        } else
            printf("%s\n", create_user_error); 

            
    } else if (strcmp(token, deposit) == 0) {
        token = strtok(NULL, space);
        if (token != NULL) {
            strcpy(user_name, token);
            if (list_find(bank->users, user_name) == NULL) 
                printf("%s\n", "No such user");
            else {
                token = strtok(NULL, space);
                if (token == NULL)
                    printf("%s\n", deposit_error);
                else {
                    unsigned int deposit = atoi(token);
                    if (strtok(NULL, space) != NULL)
                        printf("%s\n", deposit_error);
                    else {
                        unsigned int *cur_b = list_find(bank->users, user_name);
                        unsigned int new_b = *cur_b + deposit;
                        *cur_b = new_b;

                        printf("$%d %s %s's account\n", deposit, "added to", user_name);



                    }
                }
            }
        } else
            printf("%s\n", deposit_error);
    } else if (strcmp(token, balance) == 0) {
        // char user_name[1024];
        token = strtok(NULL, space);
        strcpy(user_name, token);

        if (strtok(NULL, space) != NULL) {
            printf("%s\n", balance_error);
        } else {

            if (list_find(bank->users, user_name) == NULL) {
                printf("%s\n", "No such user");
            } else { 
                unsigned int *b = list_find(bank->users, user_name);
                printf("$%d\n", *b);
                
            }
        }
    } else {
        printf("%s\n", invalid);
    }
}

void bank_process_remote_command(Bank *bank, char *command, size_t len)
{
    // TODO: Implement the bank side of the ATM-bank protocol

	/*
	 * The following is a toy example that simply receives a
	 * string from the ATM, prepends "Bank got: " and echoes 
	 * it back to the ATM before printing it to stdout.
	 */

    FILE *fp;
    char decrypted[1024];
    char call[1024];
                            
    sprintf(call,"echo %s |openssl enc -aes-256-cbc -d -a -pass file:%s -salt",command,bank->symm_key);

    fp =popen(call,"r");
    if(fp!=NULL) {
        const char space[] = " ";
        static char begin_session[] = "begin-session";
        static char withdraw[] = "withdraw";
        static char balance[] = "balance";

        fgets(decrypted,1024,fp);
        char *token;
        strtok(decrypted, "\n");
        token = strtok(decrypted, space);

        if (strcmp(token, begin_session) == 0) {
            token = strtok(NULL, space);

            char sendline[1024];
            char response[4];
            if (list_find(bank->users, token) == NULL)
                sprintf(response, "%s", "no");
            else
                sprintf(response, "%s", "yes");

            //To encrypt and make system call to create .card file
            sprintf(call,"echo %s |openssl enc -aes-256-cbc -e -a -pass file:%s -salt -out %s",response, bank->symm_key,"temp.txt");
            system(call);

            FILE *dec;
            char cipher[1024];

            //open .card file and read in cipher
            dec = fopen("temp.txt", "r");
            fgets(cipher, 1024, (FILE*)dec);            
            strtok(cipher, "\n");
            fclose(dec);
            bank_send(bank, cipher, strlen(cipher));
            system("rm temp.txt");

        } else if (strcmp(token, withdraw) == 0) {
            char *user_name;
            char *amount;
            user_name = strtok(NULL, space);
            amount = strtok(NULL, space);

            unsigned int withdraw_amt = atoi(amount);
            unsigned int *cur_b = list_find(bank->users, user_name);

            char sendline[1024];
            char response[4];

            if (*cur_b >= withdraw_amt) {
                unsigned int new_b = *cur_b - withdraw_amt;
                *cur_b = new_b;
                sprintf(response, "%s", "yes");
            } else {
                sprintf(response, "%s", "no");
            }

            //To encrypt and make system call to create .card file
            sprintf(call,"echo %s |openssl enc -aes-256-cbc -e -a -pass file:%s -salt -out %s",response, bank->symm_key,"temp.txt");
            system(call);

            FILE *dec;
            char cipher[1024];

            //open .card file and read in cipher
            dec = fopen("temp.txt", "r");
            fgets(cipher, 1024, (FILE*)dec);            
            strtok(cipher, "\n");
            fclose(dec);
            // printf("cipher is: %s\n", cipher);
            bank_send(bank, cipher, strlen(cipher));
            system("rm temp.txt");
            
        } else if (strcmp(token, balance) == 0){
            char *user_name;
            char response[1024];
            user_name = strtok(NULL, space);
            unsigned int *cur_b = list_find(bank->users, user_name);
            
            sprintf(response, "%u", *cur_b);
            // printf("response is: %s\n", response);
            //To encrypt and make system call to create .card file
            sprintf(call,"echo %s |openssl enc -aes-256-cbc -e -a -pass file:%s -salt -out %s", response, bank->symm_key,"temp.txt");
            system(call);
            FILE *dec;
            char cipher[1024];

            //open .card file and read in cipher
            dec = fopen("temp.txt", "r");
            fgets(cipher, 1024, (FILE*)dec);            
            strtok(cipher, "\n");
            fclose(dec);
            // printf("cipher is: %s\n", cipher);
            bank_send(bank, cipher, strlen(cipher));
            system("rm temp.txt");
        }

        // if (strcmp())
        // printf("name and amount are: %s",decrypted); 

    }else {
        printf("Error couldn't open .card file");
    }
                            
    pclose(fp);

	/*
    char sendline[4];
    memset(sendline, 0x00, strlen(sendline));
    command[len]=0;
    strtok(command, "\n");
    if (list_find(bank->users, command) == NULL)
        sprintf(sendline, "%s", "no");
    else
        sprintf(sendline, "%s", "yes");
    bank_send(bank, sendline, strlen(sendline));
    // printf("Received the following:\n");
    // fputs(command, stdout);
    */
	
}
