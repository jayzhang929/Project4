//int command that takes one command line arguement to create two files.

#include <stdio.h>
#include <unistd.h>

int main(int arg, char**argv) {
	FILE *atmf, *bankf;
	char *atmp, *bankp;
	int i, random;

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

         //Check if file already exits
	if(access(bankp,F_OK) != -1 ||access(atmp,F_OK) !=-1) {
 	    printf("Error: one of the file already exists\n");
	    return 63;
	}

          //Do symmetric encryption
        	
         //open the files to write to them
  	if((bankf = fopen(bankp, "w")) == NULL || (atmf = fopen(atmp, "w")) == NULL ) {
	  printf("Error creating initialization files\n");
	  return 64;
	}
           fputs("BANK",bankf);
	    fputs("ATM",atmf);
          //Do data transfer
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
