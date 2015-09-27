#include "authentication.h"

bool authenticate(char *username, char *password)
{
	FILE *file;
	char file_username[8];
	char file_password[6];

	/* !!! REMOVE THE bin/ FROM FILE NAME BEFOER SUBMISSION. Only there for convenience while testing. */
	if ((file = fopen("bin/Authentication.txt", "r")) == NULL) {
		perror("fopen");
		return false;
	}

	while (fgetc(file) != '\n'); /* Move past the column titles in the file. */

	/* Each line in the file contains the username and password separated by a space. */
	while (fscanf(file, "%s %s\n", file_username, file_password) > 0) {
		/* Check if the username matches one in the file, and if the password matches for that username. */
		if (strcmp(file_username, username) == 0 && strcmp(file_password, password) == 0) {
			return true;
		}
	}

	return false; /* No matches found. */
}
