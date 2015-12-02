#include "bank.h"
#include "ports.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

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
        char pin[5];
        int balance;

        // get user name
        token = strtok(NULL, space);

        
        if (token != NULL) {
            
            strcpy(user_name, token);

            // get user pin number
            token = strtok(NULL, space);

            
            if (token != NULL) {
                strcpy(pin, token);
                // get user balance
                token = strtok(NULL, space);

                if (token != NULL) {
                    balance = atoi(token);
                    token = strtok(NULL, space);

                    if (token != NULL) {
                        printf("%s\n", create_user_error);
                    } else {
                    
                        char *key;
                        key = (char *) malloc(sizeof(user_name) * sizeof(char));
                        strcpy(key, user_name);
                        if (list_find(bank->users, key) == NULL) {
                            
                            list_add(bank->users, key, "true");
                            
                            // create card file name
                            strcpy(file_name, user_name);
                            strcat(file_name, ".card");
                            // write pin number and balance (with an empty space in between) into the file
                            FILE *fp;
                            fp = fopen(file_name, "w+");
                            if (fp == NULL) {
                                printf("%s ", error_file);
                                printf("%s\n", user_name); 
                            } else {
                                fprintf(fp, "%s ", pin);
                                fprintf(fp, "%d", balance);
                                printf("%s ", "Created user");
                                printf("%s\n", user_name);
                            }

                            fclose(fp);
                        
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
                    int deposit = atoi(token);
                    if (strtok(NULL, space) != NULL)
                        printf("%s\n", deposit_error);
                    else {
                        FILE *fp;
                        char buff[1024];
                        strcpy(file_name, user_name);
                        strcat(file_name, ".card");

                        // open file and read in pin and current balance
                        fp = fopen(file_name, "r");
                        fgets(buff, 1024, (FILE*)fp);
                        char *pin = strtok(buff, space);
                        char *current_balance = strtok(NULL, space);
                        int cur_balance = atoi(current_balance);
                        cur_balance = cur_balance + deposit;
                        fclose(fp);

                        // rewrite to file with old pin and new balance
                        fp = fopen(file_name, "w+");
                        if (fp == NULL) {
                            printf("%s ", error_file);
                            printf("%s\n", user_name); 
                        } else {
                            fprintf(fp, "%s ", pin);
                            fprintf(fp, "%d", cur_balance);
                            printf("$%d %s %s's account\n", deposit, "added to", user_name);
                        }

                        fclose(fp);

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
                FILE *fp;
                char buff[1024];
                strcpy(file_name, user_name);
                strcat(file_name, ".card");

                fp = fopen(file_name, "r");
                fgets(buff, 1024, (FILE*)fp);
                token = strtok(buff, space);
                char *current_balance = strtok(NULL, space);
                printf("$%s\n", current_balance);

                fclose(fp);
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

	/*
    char sendline[1000];
    command[len]=0;
    sprintf(sendline, "Bank got: %s", command);
    bank_send(bank, sendline, strlen(sendline));
    printf("Received the following:\n");
    fputs(command, stdout);
	*/
}
