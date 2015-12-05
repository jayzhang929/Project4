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
	 	FILE *fp,*dec;
	    char buff[1024],userpin[1024],userpintemp[1024];
		char user_name[1024],call[1024];
		char file_name[1024],balbuf[1024],cipher[1024];
		char response[1024];
	    const char space[] = " ";
	    char *token,*pin,*pinperm;
		int cur_balance ;
	    strtok(command, "\n");
	    token = strtok(command, space);

	    // sscanf(command, "%s %s",argv1,argv2);
	      

	    //begin-session

	    if(strcmp(token, "begin-session") == 0) {
			//Check to see no other user is logged in	 	
			if(atm->session == 1) {
				printf("A user is already logged in\n");
				return;		
			}

	        //get user name
		    token = strtok(NULL, space);
	          
			if (token != NULL) {
				    strcpy(user_name, token);
				       
		            //encrypt username
				    memset(balbuf,0x00,strlen(balbuf));
		            memset(call,0x00,strlen(call));			
				    sprintf(balbuf,"%s %s","begin-session",user_name);
					       
		            //To encrypt and make system call to create .card file
					sprintf(call,"echo %s |openssl enc -aes-256-cbc -e -a -pass file:%s  -out %s",balbuf,atm->symm_key,"command.txt");
					system(call);
		                                 
					//get decrypted file and send it
					dec = fopen("command.txt", "r");
						if(dec != NULL) {
							fgets(cipher, 1024, (FILE*)dec);
					
							//strtok(cipher, "\n"); 
							             
			            } else {
							printf("Unable to access %s's card\n",user_name);
			                return;
						}			
					fclose(dec);
                    char resp [1024];
				    memset(response,0x000,strlen(response));
				    atm_send(atm, cipher, strlen(cipher));
		    		atm_recv(atm,resp,1024);
					//strtok(response, "?");		            
		            // printf("%s\n", resp);
					//strcpy(response, "U2FsdGVkX18ds9QlHghY0rtEQqPxzgsPOGp4tdTq3ZM=");


				 	//decrypt response

					//make call command
					sprintf(call,"echo %s |openssl enc -aes-256-cbc -d -a -pass file:%s  ",resp,atm->symm_key);
                    //Get command from console
				    fp =popen(call,"r");
	                              
		
                          //if all goes well 
				    	if(fp!=NULL) {
							    fgets(buff,1024,fp);                                  
								//Can now use decrypted text for whatever
								//printf("%s",buff); 
								    //Check response 
								strtok(buff,"\n");
				  		        if(strcmp(buff, "yes") == 0){
						            //printf("WHAT\n");
						            //Search for .card file			         
				                    strcpy(file_name, user_name);
				                    strcat(file_name, ".card");
				                           
				                    // open file and read in pin and current balance

				                              //TO decrypt				
									//char cipher[1024];
									//flush cipher and call
							  		memset(cipher,0x00,strlen(cipher));                          
				                    memset(call,0x00,strlen(call));			
						          	memset(buff,0x00,strlen(buff));
							    

									//open .card file and read in cipher
									dec = fopen(file_name, "r");
									if(dec != NULL) {  //if for opening cipher
								  		fgets(cipher, 1024, (FILE*)dec);
								  		strtok(cipher, "\n");
				                                                            //make call command
										sprintf(call,"echo %s |openssl enc -aes-256-cbc -d -a -pass file:%s -salt 2>&1",cipher,atm->symm_key);
				                                //Get command from console
										fp =popen(call,"r");             
					                        //if all goes well 
										if(fp!=NULL) {
								    			fgets(buff,1024,fp);                                  
												//Can now use decrypted text for whatever
										  		//printf("%s",buff); 
					                                  //TODO change \n
										  		pin = strtok(buff,"\n");
									             //Prompt for user's pin
												printf("PIN? ");
												fgets(userpin,1024,stdin);
												strtok(userpin, "\n");
												//compare with pin on file
												//printf("%s Got %s\n", pin, userpin);
								  			           	if(strcmp(userpin,pin) ==0) {	
								        	    	        printf("Authorized\n");	
															//start session
															strncpy(atm->username,user_name,strlen(user_name));
															atm->session = 1;
								        		            return;
									
														} else {
											 				printf("Not authorized\n");
											 				return;				 
														}

				                    	
					                	} else { //ENDIF fro pinread
									 		printf("Unable to access %s's card\n",user_name);
								         	return;
										}

				                         pclose(fp);

					                } else { //ENDIF for opening pin cipher
									   printf("Unable to access %s's card\n",user_name);
					                   return;
									}			
								
									fclose(dec);   

								} else { //END IF FOR buff == yes
								    printf("No such user\n");
							    	return;
								}                       
				     
							} else { //ENDFOR if all goes well
						     	printf("No such user\n");
								return; 
						
						 	} 
						

            }else {
	            printf("Usage: begin-session <user-name>\n");
			 	return;		
			} 

			         

		 } else if(strcmp(token, "withdraw") == 0 ) { //withdraw command

			//User has been verified and can now get to conduct other transactions
	               if(atm->session == 1) {
	 	                 //Yeah get me my cash real quick!!!
				     token = strtok(NULL, space);
	                  

				 if (token == NULL) {
			             printf("Usage: withdraw <amt>\n");
				     return;
				  }else {                            
				        int amount =atoi(token);
					  if(amount >cur_balance) {
						//Go work pal
					   	printf("Insufficient funds\n");
		 		          }else { 
	                                        FILE *fp;
	 					//Reduce the balance
						cur_balance = cur_balance - amount;

						/* rewrite to file with new balance
					        fp = fopen(file_name, "w+");
					        if (fp != NULL) {
	                                            fprintf(fp, "%s ", pin);
					            fprintf(fp, "%d", cur_balance);
					            printf("$%d dispensed\n",amount);
					        }
					        fclose(fp);*/
					memset(balbuf,0x00,strlen(balbuf));			
					sprintf(balbuf,"%s %d",pin,cur_balance);
				       
	                          //To encrypt and make system call to create .card file
					sprintf(call,"echo %s |openssl enc -aes-256-cbc -e -a -pass file:%s -salt -out %s",balbuf,atm->symm_key,file_name);
					system(call);
	                                 //flush call 
					memset(call,0x00,strlen(call));
					
					 printf("$%d dispensed\n",amount);
					  }				   
					return;
				    }
	                
	               }else {
	             	  printf("No user logged in\n");
			  return;	
		       }
	          }else if(strcmp(token, "balance") == 0 ) { //balance command

			//User has been verified and can now get to conduct other transactions
	               if(atm->session == 1) {
	 	                 //Yeah get me my balance real quick
				 if (strtok(NULL, space) != NULL) {
			             printf("Usage: balance\n");
				     
				  }else {
				   printf("$%d\n",cur_balance);
				   
				 }
	                       return;
	               }else {
	             	  printf("No user logged in\n");
			  return;	
		       }
	          } else if(strcmp(token, "end-session") == 0 ) { //end-session command

			//User has been verified and can now get to conduct other transactions
	               if(atm->session == 1) {
	 	                 //Bye-am out of here ...Thanks though!!			 
	                     atm->session = 0;
	                     //strcpy(file_name,"");
			     //strcpy(user_name,"");
			      memset(file_name,0x00,strlen(file_name));
	                      memset(user_name,0x00,strlen(user_name));
	 		      memset(atm->username,0x00,251);
			    // cur_balance = 0;
		             printf("User logged out\n");			  
	  		     return;                
	               }else {
	             	  printf("No user logged in\n");
			  return;	
		       }
			
	           } else {
	                 printf("Invalid command\n");
	  		return;
	           }
		

	}

