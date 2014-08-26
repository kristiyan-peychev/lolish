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

#ifndef _header
#define _header
#define	_LOLISH_VERSION	"1.2.2 RC"
#define	DIR_FLAGS
	#define PRINT_HIDDEN 	1
	#define SORT		2
	#define PRINT_FILE_ONLY	4
	#define PRINT_DIRS_ONLY	8
#define	EXEC_FLAGS
	#define EXEC	1
	#define COMP	2
	#define NOT_RUNNING_UNDER_BASH	4
#define	RC_FLAGS
	#define	_LOLISH_NO_FILE	0001
	#define	_LOLISH_NO_DIR	0002 | _LOLISH_NO_FILE
	#define _LOLISH_COMMENT	0004
char	**getCommands(int *);
int	getCommandsFromDIR (DIR *, char **, short);
int	readPATH(char **);
int	_lolish_refresh(void);
int	_lolish_loadrc(char *);
int	_lolish_make_basic_rc(int);
void	_lolish_tpl(char *);

#endif

