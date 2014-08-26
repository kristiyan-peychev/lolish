/****            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE                 **
**                         Version 2, December 2004                          **
**        Copyright (C) 2013 Kristiyan Peychev <kurotsuki_l@yahoo.com>       **
**                                                                           **
**      Everyone is permitted to copy and distribute verbatim or modified    **
**      copies of this license document, and changing it is allowed as long  **
**      as the name is changed. More info about the license at               **
**      http://www.wtfpl.net/                                                **
**                                                                           **
**                DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE                **
**       TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION     **
**                0. You just DO WHAT THE FUCK YOU WANT TO.                  **
**                                                                          **/

#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int	getCommandsFromDIR(DIR *dp, char **cp, short writeFromWhere){
	struct dirent *ep;
	short num_written = writeFromWhere, recurse = 0;
	if (dp != NULL) {
		while ((ep = readdir(dp))) {
			if (ep->d_type == DT_REG || ep->d_type == DT_LNK) {
				sprintf(*(cp + num_written), "%s%c", ep->d_name, 0x0);
				num_written++;
			} else {
			}
		}
	} else
		;
return (num_written - writeFromWhere);
}

int	readPATH(char **dirs){
	int	i = 0, j = 0, k = 0;
	char	buffer[256], *pathval = getenv("PATH");
	while (*(pathval + i)) {
		if (*(pathval + i) == ':') {
			*(buffer + k) = 0;
			k++;
			k = 0;
			*(dirs + j) = (char *) malloc(64 * sizeof(char));
			strcpy(*(dirs + j), buffer);
			memset(buffer, 0, 256);
			j++;
			i++;
			continue;
		}
		*(buffer + k) = *(pathval + i);
		i++;
		k++;
	}
return	j;
}

char	**getCommands(int *totWrit){
	char 	**dirs = malloc(24 * sizeof(char *)),
		**commands;
	int	totalWritten = 0, size = 8120, i = 0, somecount = 0;
	if ((somecount = readPATH(dirs)) < 0) {
		fprintf(stdout, "lolish: Error reading PATH!\n");
		return NULL;
	}
	commands = (char **) malloc(size * sizeof(char *));
	for (i = 0; i < size; i++) 
		*(commands+i) = (char *) malloc(48 * sizeof(char));
	DIR *dp;
	i = 0;
	while (i < somecount) {
		dp = opendir(*(dirs + i));
		if (dp == NULL) {
			fprintf(stdout, "lolish: Failed in reading %s\n", *(dirs + i));
			i++;
			continue;
		}
		totalWritten += getCommandsFromDIR(dp, commands, totalWritten);
		closedir(dp);
		i++;
	}
	*totWrit = totalWritten;
return commands;
}

