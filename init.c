//int command that takes one command line arguement to create two files.

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(int arg, char**argv) {
	FILE *atmf, *bankf,*key;
	char *atmp, *bankp,*line = NULL;
	size_t len = 0;
	ssize_t read;
	
	if(arg !=2) {
	printf("Usage:init<filename>\n");
	return 62;
	}

	
	//Allocate memory
	bankp = malloc(strlen(argv[1])+strlen(".bank")+1);
	atmp = malloc(strlen(argv[1])+strlen(".atm")+1);
        
        //Concat for the file names
	if(bankp != NULL || atmp != NULL) {
	sprintf(bankp,"%s%s",argv[1],".bank");
	sprintf(atmp,"%s%s",argv[1],".atm");
	}
               //Remember to delete this ..just deletes the mad files for now
               char command[50];
		char comman[50];
		strcpy(command,"rm -f file.bank");
                strcpy(comman,"rm -f file.atm");

        	system(command);
                system(comman); 

         //Check if file already exits
	if(access(bankp,F_OK) != -1 ||access(atmp,F_OK) !=-1) {
 	    printf("Error: one of the file already exists\n");
	    return 63;
	}
                 
          //Do symmetric encryption
		//system("sudo ~/.rnd");
                system("openssl rand -base64 2048 -out symm_key &>/dev/null");
                key = fopen("symm_key","r"); 
                
		
      	//system("openssl enc -aes-256-cbc -e -in plaintext.txt -out cipher.bin -pass file:symm_key -salt");
         //open the files to write to them
  	if((bankf = fopen(bankp, "w")) == NULL || (atmf = fopen(atmp, "w")) == NULL ) {
	  printf("Error creating initialization files\n");
	  return 64;
	}   


		if(key != NULL) {
	 		while ((read = getline(&line,&len,key)) != -1) {
				//printf("%s",line);					
	    			//Do key transfer to both bank and atm file 
          			 fputs(line,bankf);
	   			 fputs(line,atmf);
			}
		}else {
		     printf("Error creating initialization files\n");
	  	     return 64;
		}
	
            //Close the files
        if(fclose(bankf) || fclose(atmf)) {
	  printf("Error creating initialization files\n");
	  return 64;
	}

	//Free allocated memory
	free(bankp);
	free(atmp);
        
        //YIPPEE ...SUCCESS
	printf("Successfully initialized bank state\n");

return  0;
}


