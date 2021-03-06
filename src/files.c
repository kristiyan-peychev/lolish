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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "header.h"

int	_lolish_make_basic_rc(rc_fd)
	int	rc_fd;
{
	char eofchar = EOF;
	char basic_rc_prototype[] = "## LolitaShell rc file;\n## Not really the best thing I can manage to write.\n## You must have a cd call at the beginning for the shell to run\n## normally. I can guarantee nothing if you do not have one.\n## Generated by LOLITA SHELL\ncd .\n## EOF\n";
	write(rc_fd, basic_rc_prototype, sizeof(basic_rc_prototype));
	write(rc_fd, &eofchar, 1);
return 0;
}

int	_lolish_loadrc(rc_loc)
	char	rc_loc[];
{
	char owd[256], buf[256], *uname = getlogin();
	struct dirent *ep;
	int _rc, flg = 0, i = 0;
	char *line_buffer, rc_file[512];
	DIR *dp;
	getcwd(owd, 256);
	if (*rc_loc == 0x7e) {
		strcpy(buf, rc_loc + 1);
		strcpy(rc_loc, "/home/");
		strcat(rc_loc, uname);
		strcat(rc_loc, buf);
	} else {
		fprintf(stdout, "lolish: Wrong directory!\n");
		return 1;
	}
	dp = opendir(rc_loc);
	if (chdir(rc_loc)) {
		mkdir(rc_loc, 0777);
		flg |= _LOLISH_NO_DIR;
		closedir(dp);
		dp = opendir(rc_loc);
	}
	chdir(owd);
	if (dp == NULL) {
		printf("lolish: Something's wrong with the RC directory\n");
		closedir(dp);
		return -1;
	}
	while ((ep = readdir(dp))) {
		if (!strcmp("_lolishrc", ep->d_name)) {
			flg = 0;
			break;
		} else {
			flg |= _LOLISH_NO_FILE;
		}
	}
	strcpy(rc_file, rc_loc);
	strcat(rc_file, (*(rc_loc + (strlen(rc_loc))) == '/' ? "_lolishrc" : "/_lolishrc"));
	if (flg & _LOLISH_NO_FILE) {
		_rc = open(rc_file, O_WRONLY | O_CREAT, 0777);
		_lolish_make_basic_rc(_rc);
		close(_rc);
		flg ^= _LOLISH_NO_FILE;
	}
	_rc = open(rc_file, O_RDONLY);
	line_buffer = (char *) malloc(4096 * sizeof(char));
	while (read(_rc, line_buffer + i, 1) > 0) {
		if (*(line_buffer + i) == '\n' || *(line_buffer + i) == EOF) {
			if (*(line_buffer + i) == EOF && !strcmp(line_buffer, "")) 
				break;
			*(line_buffer + i) = 0;
			i = 0;
			if (flg & _LOLISH_COMMENT){
				flg ^= _LOLISH_COMMENT;
				continue;
			} else {
				commandCheck(line_buffer, EXEC);
				continue;
			}
		} else if (i == 0) {
			if (*line_buffer == '#') {
				flg |= _LOLISH_COMMENT;
			} 
		}
		i++;
	}
	free(line_buffer);
	close(_rc);
	closedir(dp);
return 0;
}

