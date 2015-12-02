/* 
 * The main program for the ATM.
 *
 * You are free to change this as necessary.
 */

#include "atm.h"
#include <stdio.h>
#include <stdlib.h>

static const char prompt[] = "ATM: ";

int main(int arg, char **argv)
{
    char user_input[1000];

    ATM *atm = atm_create();
   
    //Check for command line arguements
     atm->init = fopen(argv[1],"r");
    if(arg != 2||atm->init ==NULL) {
    	printf("Error opening atm initialization file\n");
	return 64;
    }
    printf("%s", prompt);
    fflush(stdout);

    while (fgets(user_input, 10000,stdin) != NULL)
    {
        atm_process_command(atm, user_input);
	if(atm->session == 1) {
		printf("ATM (%s); ",atm->username);	
	} else {
        printf("%s", prompt);
	}
        fflush(stdout);
    }   
	atm_free(atm);
	return EXIT_SUCCESS;
}
