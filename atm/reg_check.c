#include <string.h>
#include <stdbool.h>

bool user_name_check (char *user_name) {
	if (strlen(user_name) > 250)
		return false;

	int i;
	for (i = 0; i < strlen(user_name); i++)
		if ((int)user_name[i] < 65 
			|| ((int)user_name[i] > 90 && (int)user_name[i] < 97)
			|| (int) user_name[i] > 122)
			return false;

	return true;
}

bool withdraw_check (char *pin) {
	if (strlen(pin) > 10)
		return false;

	int i;
	for (i = 0; i < strlen(pin); i++)
		if ((int)pin[i] < 48 || (int)pin[i] > 57)
			return false;

	return true;
}