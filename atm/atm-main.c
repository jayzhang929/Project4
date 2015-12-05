/* 
 * The main program for the ATM.
 *
 * You are free to change this as necessary.
 */

#include "atm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char prompt[] = "ATM: ";

int main(int arg, char **argv)
{
    char user_input[1000];
    if (arg != 2) {
        printf("Error opening atm initialization file\n");
        return 64;
    }
    char *token;
   char argv_cp[1024];
   strcpy(argv_cp, argv[1]);
   strtok(argv_cp, ".");
   token = strtok(NULL, ".");
   if (strcmp(token, "atm") != 0) {
    printf("Error opening atm initialization file\n");
    return 64;
   }

    ATM *atm = atm_create();
   
    //Check for command line arguements
     FILE *fp;
     fp = fopen(argv[1],"r");
    if(fp ==NULL) {
    	printf("Error opening atm initialization file\n");
	return 64;
    }
    //strcpy(atm->symm_key, argv[1]);
    fclose(fp);
    printf("%s", prompt);
    fflush(stdout);

    while (fgets(user_input, 10000,stdin) != NULL)
    {
        atm_process_command(atm, user_input);
	if(atm->session == 1) {
		printf("ATM (%s): ",atm->username);	
	} else {
        printf("%s", prompt);
	}
        fflush(stdout);
    }   
	atm_free(atm);
	return EXIT_SUCCESS;
}
