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
#include <termio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "header.h"

int	catchIn(void){
	struct termios oldt, newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getc(stdin);
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
} /* Get character without entering. High level. About to be updated to lower level :) */

char	*userName,
	*CWD;

int	execFlgs = 0,
	EX = 0;

char	*getCWDIR(void){	/* Get the name of the current working directory */
	static char CWD[256];	/* and convert it to something that looks better */
	char buf[256] = "", buf0[16] = "", buf1[256] = "", getbuf[512] = "";
	int h = 0, fl = 0, g = 0, flag = 0;;
	memset(CWD, 0, 255);
	getcwd(buf, 256);
	while (buf[h]) {
		fl += (buf[h] == '/' ? 1 : 0);
		h++;
	}
	h = 0;
	while (g < 5) {
		buf0[g] = buf[g];
		g++;
	}
	if (!strcmp(buf0, "/home")) 
		flag++;
	else {
		strcpy(CWD, buf);
		return CWD;
	}
	g++;
	while (buf[g] != '/') {
		if (buf[g] == '\0') 
			break;
		buf1[h] = buf[g];
		g++;
		h++;
	}
	h = 0;
	if (!strcmp(buf1, userName)) {
		while (g < strlen(buf)) {
			getbuf[h] = buf[g];
			h++;
			g++;
		}
		*CWD = '~';
		strcat(CWD, getbuf);
	} else {
		getcwd(CWD, 256);
		return CWD;
	}
return CWD;
}

int 	numLastWritten = 0;
DIR     *currentDIR;
char    **currentDIRVal;
/*
void	loadDIRVal(void){
	int i = 0;
	currentDIRVal = (char **)(numLastWritten ? realloc(currentDIRVal, times * 4096 * sizeof(char *)) : malloc(times * 4096 * sizeof(char *)));
	for (i = 0; i < times * 4608; i++) {
		*(currentDIRVal+i) = (char *)(numLastWritten ? realloc(*(currentDIRVal + i), 96 * sizeof(char)) : malloc(96 * sizeof(char)));
		memset(*(currentDIRVal + i), 0, 96);
	}
}
*/
void	clearCurrentDIRVal(void){
	int i = 0;
	for (i = 0; i < numLastWritten; i++) 
		memset(*(currentDIRVal + i), 0, 96 * sizeof(char));
}

int	allocDIRV(size) /* Allocater function for the directory values */
	int size;
{
	int i = 0;
	volatile static long int lastSize = 64;
	if (numLastWritten == 0) {
		currentDIRVal = (char **) malloc(lastSize * sizeof(char *));
		for (i = 0; i < 32; i++) {
			*(currentDIRVal + i) = (char *) malloc(96 * sizeof(char));
			memset(*(currentDIRVal + i), 0, 96);
		}
	} else {
		if (size <= lastSize) 
			return -1;
		char **uberbuffer = (char **) malloc((numLastWritten + 1) * sizeof(char *));
		memset(uberbuffer, 0, (numLastWritten + 1) * sizeof(char *));
		for (i = 0; i < numLastWritten; i++) {
			*(uberbuffer + i) = (char *) malloc(96 * sizeof(char));
			if (strcmp(*(currentDIRVal + i), "") || **(currentDIRVal + i) != 0) {
				strcpy(*(uberbuffer + i), *(currentDIRVal + i));
			} else {
				memset(*(uberbuffer + i), 0, 96);
			}
			//memset(*(currentDIRVal + i), 0, 96 * sizeof(char));
			free(*(currentDIRVal + i));
		}
		//memset(currentDIRVal, 0, (numLastWritten + 1) * sizeof(char));
		free(currentDIRVal);
		currentDIRVal = (char **) malloc(size * sizeof(char *));
		for (i = 0; i < size; i++) {
			*(currentDIRVal + i) = (char *) malloc(96 * sizeof(char));
			if (i >= numLastWritten){
				memset(*(currentDIRVal + i), 0, 96);
			} else if (strcmp(*(uberbuffer + i), "") || **(uberbuffer + i) != 0) {
				strcpy(*(currentDIRVal + i), *(uberbuffer + i));
			} else { 
				memset(*(currentDIRVal + i), 0, 96);
			}
			if(i < numLastWritten){
				//memset(*(uberbuffer + i), 0, 96);
				free(*(uberbuffer + i));
			}
		}
		free(uberbuffer);
		lastSize = size;
	}
return 0;
}

void	getCurrentDIRVal(void){
	struct dirent *ep;
	int looper = 0, alloc = 32;
	allocDIRV(alloc);
	while ((ep = readdir(currentDIR))) {
		if (looper == alloc - 1) {
			alloc += 16;
			allocDIRV(alloc);
		}
		if (!strcmp(ep->d_name, "")) 
			continue;
		*(currentDIRVal + looper) = (char *) malloc(96 * sizeof(char));
		strcpy(*(currentDIRVal + looper), ep->d_name);
		looper++;
	}
	numLastWritten = looper;
return;
}

void	printFullDIR(flags)	/* Print the current working directory contents (ls) */
	int flags;
{
	fprintf(stdout, "Printing `%s` %d\n", getCWDIR(), numLastWritten);
	int i = 0;
	while (i < numLastWritten - 1) {
		printf("%s\t", *(currentDIRVal + i));
		i++;
	}
	printf("\n");
}

									/* Built-in functions */

void	quadratic(void){	/* A quadratic equasion computing function, incomplete so doesn't work */
	double x[2], D, a, b, c;
	int n = 0, i = 0;
	char inCh, chch = 0, stack[16];
	do {
		if (i == 0) {
			if (n == 0) {
				printf("a = ");
				fflush(stdout);
			}
			chch = 'a';
		}
		else if (i == 1) {
			if (n == 0) {
				printf("b = ");
				fflush(stdout);
			}
			chch = 'b';
		}
		else if (i == 2) {
			if (n == 0) {
				printf("c = ");
				fflush(stdout);
			}
			chch = 'c';
		}
		inCh = catchIn();
		if (inCh == 32) {
			stack[n] = 0;
			n = 0;
			a = chch != ('a' ? : atof(stack));
			b = chch != ('b' ? : atof(stack));
			c = chch != ('c' ? : atof(stack));
			i++;
			printf(" ");
		}
		else {
			printf("%c", inCh);
			stack[n] = inCh;
			n++;
		}
	} 
	while (i <= 2);
	char p;
	if (round(b/2) == b/2) {
		p = 0;
		D = (b*b-4*c*a);
	}
	else {
		p = 1;
		D = (b/2)*(b/2)-a*c;
	}
	printf("D = %f\n", D);
	if (p == 0) {
		x[0] = (-b+sqrt(D))/2*a;
		x[1] = (-b-sqrt(D))/2*a;
	}
	else {
		x[0] = (-(b/2)+sqrt(D))/a;
		x[1] = (-(b/2)+sqrt(D))/a;
	}
	printf("x1 = %f; x2 = %f\n", x[0], x[1]);
} // Strangely overlycomplicated function

void 	destroy(void *dummyPointer) { /* Function intended to free all top-level arrays */
	fprintf(stdout, "Exiting\n"); /* but I'll get to that later */
	exit(1);
return;
}

void	test(void *args) {
	char h = 'h';
return;
}

char	**aliasList = NULL;
int	alias_c = 0;

void 	_lolish_alias(void *args) { /* Adds an alias of a command string in the alias list */
	static int list_size = 0;
	char *argv = (char *) args, buffer[128] = "";
	int i = 0, flg = 0, c = 0;
	while (*argv == ' ')
		++argv;
	while (1) {
		if (*(argv + i) == '\\') 
			i++;
		if (*(argv + i) == ' ' || *(argv + i) == ':' || *(argv + i) == '=') {
			*(buffer + c) = 0;
			i++;
			while (*(argv + i) == ' ' || *(argv + i) == ':' || *(argv + i) == '=') 
				i++;
			break;
		}
		*(buffer + c) = *(argv + i);
		i++;
		c++;
	}
	list_size += 2;
	aliasList = (char **) realloc(aliasList, list_size * sizeof(char *));
	*(aliasList + list_size - 2) = (char *) malloc(strlen(argv) * sizeof(char));
	memset(*(aliasList + list_size - 2), 0, strlen(argv) * sizeof(char));
	*(aliasList + list_size - 1) = (char *) malloc(strlen(argv) * sizeof(char));
	memset(*(aliasList + list_size - 1), 0, strlen(argv) * sizeof(char));
	argv += i;
	strcpy(*(aliasList + list_size - 2), buffer);
	strcpy(*(aliasList + list_size - 1), argv);
	alias_c += 1;
return;
}

void	horner(void *FOREVER_USELESS){	/* Just as arguments suggest, I'd planned something */
	char h = 'h';			/* for this function, but I gave up after making progress */
return;					/* in the important segments of the shell :) */
}

void 	shif(void *args){
	char h = 'h';
return;
}

void 	shfor(void *args){
	char h = 'h';
return;
}

void 	cd(void *args){			/* Standart change directory built-in function */
	char *argv = (char *) args, owd[256], buf[256];
	int i = 0, dirup = 0, ss = 0, tss;
	getcwd(owd, 256);
	if (!strcmp(argv, "")) {
		strcpy(buf, argv + 1);
		strcpy(argv, "/home/");
		strcat(argv, userName);
		memset(buf, 0, 256);
	} else if (*(argv) == 0x7e) {
		strcpy(buf, argv + 1);
		strcpy(argv, "/home/");
		strcat(argv, userName);
		memset(buf, 0, 256);
	} else if (*(argv) == 0x2e) {
		if (*(argv + 1) == 0x2e) {
			dirup++;
			if (*(argv + 2) == 0x2f) {
				i = 3;
				while (*(argv + i)) {
					if (*(argv + i) == 0x2e && *(argv + (i++)) == 0x2e) {
						dirup++;
					}
					i++;
				}
			}
		} else {
			ss = 2;
			tss = 0;
			while (*(argv + ss)) {
				*(buf + tss) = *(argv+ss);
				ss++;
				tss++;
			}
			buf[tss] = 0;
			strcpy(argv, owd);
			strcat(argv, "/");
			strcat(argv, buf);
			strcpy(buf, "");
			ss = 0;
		}
	} else if (*(argv) == 0x2f) 
		;
	else {
		fprintf(stdout, "lolish: Wrong directory\n");
		return;
	}
	i = 0;
	while (dirup) {
		ss = strlen(owd);
		while (1) {
			if (*(owd+ss) == 0x2f) {
				break;
			} else {
				*(owd+ss) = 0;
				ss--;
			}
		}
		dirup--;
		i++;
	}
	if (i) 
		strcpy(argv, owd);
	if (!chdir(argv)) {
		currentDIR = opendir(argv);
		getCurrentDIRVal();
	} else 
		fprintf(stdout, "lolish: Could not open directory.\n");
return;
}

char	*isUserSwitched = NULL;

void	exec(void *);

void	switchUser(void *args){							/* USELESS!!! */
	char s[] = "su";
	strcat(s, args);
	#ifndef EXEC_FLAGS
	strcat(s, " -c exit");
	#else
	if (execFlgs & NOT_RUNNING_UNDER_BASH)
		;
	else
		strcat(s, " -c /home/kawaguchi/lolish/in");
	#endif
	exec(s);
	if (args == NULL) {
		isUserSwitched = (char *) malloc(5 * sizeof(char));
		isUserSwitched = "root";
	}
	
return;
}

void	version(void *argv) {							/* Print the current version on the shell (duh) */
	fprintf(stdout, "lolish: version %s\n", _LOLISH_VERSION);
return;
}

int 	commandCount = 0, command_index = 0,
	CC = 11;
char    **commands;
char    *commandDesc[] = {"exit", "quad", "test", "exec", "if", "for", "su", "cd", "ver", "version", "lalias"};			/* Command descriptors as listed below. */
void 	(*fun[32])(void *args) = {destroy, quadratic, test, exec, shif, shfor, switchUser, cd, version, version, _lolish_alias};	/* Commands as listed. */

void 	coreCMDCheck(char *, char *, short);

short	inCurDIR(char *, char *, short);

void 	splitCMDFromARGS(char *from, char *this, char *args){
	short i = 0, kk = 1, j = 0;
	char prev = 0;
	while (*(from + i)) {
		if (*(from + i) == 0x20 /*&& prev != '\\'*/ && kk){
			kk = 0;
			*(this + i) = 0;
			i++;
			continue;
		}
		if (kk) {
			*(this + i) = *(from + i);
		}
		if (!kk){
			if (*(args + j) != '\\')
				*(args + j) = *(from + i);
			j++;
		}
		prev = *(args + j);
		i++;
	}
	*(args + j) = 0;
}

void 	commandCheck(char *allz, short flags){		/* Real executor function */
	int	rot1 = 0, rot2 = 0;
	char	this[256], buffer[256], g[3], args[512];
	splitCMDFromARGS(allz, this, args);
	g[0] = allz[0];
	g[1] = allz[1];
	g[2] = 0;
	if(!strcmp(g, "./") || !strcmp(g, "||")){
		inCurDIR(this, allz, flags);
		return;
	}
	while (rot1 < CC) {
		if (!strcmp(this, commandDesc[rot1])) {
			(*fun[rot1])((void *) args);
			return;
		}
		rot1++;
	}
	if (alias_c > 0) {
		while (rot2 < alias_c) {
			if (!strcmp(this, *(aliasList + rot2))) {
				strcpy(buffer, *(aliasList + ++rot2));
				strcat(buffer, args);
				exec(*(aliasList + (++rot2)));
				//exec(buffer);
				return;
			}
			rot2 += 2;
		}
	}
	exec(allz);
return;
}

/*
void	coreCMDCheck(char *this, char *all, short flags){
	short i = 0;
	while (i < totwrit) {
		if (flags & EXEC) {
			if (!strcmp(this, *(cmdlist+i))) {
				exec(all);
				return;
			}
			i++;
		}
	}
	printf("lolish: `%s' was not found.\n", this);
}
*/

short	inCurDIR(char *this, char *all, short flags){
	short i = 2;
	char that[48] = "", *login = getlogin();
	while (i < strlen(this)) {
		that [i-2] = this [i];
		i++;
	}
	strcpy(all, "/home/");
	strcat(all, login);
	strcat(all, this);
	i = 0;
	while (i < numLastWritten) {
		if (!strcmp(that, *(currentDIRVal+i))) 
			return 0;
		i++;
	}
	fprintf(stdout, "lolish: `%s' was not found in current working directory.\n", that);
return 1;
}

void	moveVert(char is[], int i, int max){
	while (i < max) {
		*(is + i) = *(is + ++i);
	}
return;
}

void	freeIt(char arr[]){
	short k = 0;
	while (arr[k]) {
		arr[k] = 0;
		k++;
	}
return;
}

void	voidFunction(void *dummyPointer){
return;
} // Useless function.

void	parseARGV(int argc, const char *argv[]){
	int i = 0;
	while (i < argc) {
		if (!strcmp(argv[i], "--nobash")) {
			execFlgs |= NOT_RUNNING_UNDER_BASH;
		} else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--test")) {
			execFlgs |= 07777;
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || !strcmp(argv[i], "-?")) {
			printf("lolish: Usage: \n");
			printf("lolish [OPTIONS]\n");
			printf("JAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJAJA\n");
			printf("A simple and relatively \n");
			printf("lightweight unix shell.\n");
			printf("Don't expect too much out\n");
			printf("of it though, I've been working\n");
			printf("on it alone, so there are a few \n");
			printf("bugs that still need fixing.\n");
			printf("===================================\n");
			destroy(NULL);
		}
		i++;
	}
}

void	_lolish_line(void){
	char hostname[64] = "";
	gethostname(hostname, 64);
	fprintf(stdout, "%s@%s<%s>$ ", userName, hostname, getCWDIR());
	fflush(stdout);
return;
}

int	main(argc, argv)
	int		argc;
	const char	*argv[];
{
	char 	*inputString, isTabbed = 0, hostname[64] = "", *inBuf,
		rc_file_location[] = "~/.terminalcode";
	int 	inChar, escapeFlg = 0, bb = 0;
	int	i = 0, max = 0;
	isUserSwitched = NULL;
	aliasList = (char **) malloc(1);
	parseARGV(argc, argv);
	if (_lolish_refresh() < 0) printf("lolish: Failed to read the command names from PATH, only built-in commands will work\n");
	//currentDIR = opendir("."); /* Open the current directory for reading */
	//getCurrentDIRVal(); /* Load the current directory file names in the momory */
	userName = getlogin();
	gethostname(hostname, 64);
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGABRT, SIG_IGN);
	inputString = (char *) malloc(1024 * sizeof(char));
	inBuf = (char *) malloc(512 * sizeof(char));
	_lolish_loadrc(rc_file_location); /* Sorta bugged */
	do {
		_lolish_line();
		do {
			if (EX == -1) 
				break;
			inChar = catchIn();
			if (inChar == 4 && max == 0) {	/* Ctrl + D */
				voidFunction(NULL);
				destroy(NULL);
			} else if (inChar == 0xa) { /* Enter */
				*(inputString + max) = 0;
				putc(0xa, stdout);
				if (!strcmp(inputString, ""))
					break;
				commandCheck(inputString, EXEC);
				memset(inputString, 0, 1024);
				i = 0;
				max = 0;
				break;
			} else if (inChar == 9 && max == 0) { /* Tab */
				if (isTabbed) {
					isTabbed = 0;
					putc(0xa, stdout);
					printFullDIR(currentDIR, 0);
					break;
				} else 
					isTabbed++;
			} else if (inChar == 0x7f && i) { /* Backspace */
				if (i > 0) {
					if ( i == max) {
						printf("\b%c\b", 0x20);
						*(inputString + max) = 0;
					} else {
						bb = i;
						while (bb >= max) {
							*(inputString + bb - 1) = *(inputString + bb);
							*(inputString + bb) = 0;
							bb--;
						}
						bb = 0;
						while (bb <= max) {
							putc(0x20, stdout);
							bb++;
						}
						putc('\r', stdout);
						_lolish_line();
						moveVert(inputString, i-1, max);
						*(inputString + max) = 0;
						*(inputString + max - 1) = 0;
						printf("%s", inputString);
						bb = max;
						while (bb > i) {
							putc('\b', stdout);
							bb--;
						}
					}
					i--;
					max--;
					fflush(stdout);
				} else 
					continue;
			} else if (inChar == 0x7f && !i){
				continue;
			} else if (inChar == 0x1b || escapeFlg != 0) { /* Esc */
				if (escapeFlg == 0) {
					 escapeFlg = 1;
					 continue;
				} else {
					if (inChar == 0x5b && escapeFlg == 1) {	/* Special characters ( like arrows, home, end, del ) */
						escapeFlg = 2;
						continue;
					} else if (escapeFlg == 2 || escapeFlg == 4) {
						if (inChar == 0x43) {
							if (i == max) {
								escapeFlg = 0;
								continue;
							} else {
								escapeFlg = 0;
								putc(inputString[i], stdout);
								i++;
								continue;
							}
						} else if (inChar == 0x44) {
							if (i == 0) {
								escapeFlg = 0;
								continue;
							} else {
								escapeFlg = 0;
								putc(0x8, stdout);
								i--;
								continue;
							}
						} else if (inChar == 0x33 || escapeFlg == 4) {
							if (inChar == 0x7e) {
								if (i == max) {
									escapeFlg = 0;
									continue;
								}
								bb = 0;
								while (bb <= max) {
									putc(0x20, stdout);
									bb++;
								}
								putc('\r', stdout);
								_lolish_line();
								moveVert(inputString, i, max);
								*(inputString + max) = 0;
								*(inputString + max - 1) = 0;
								printf("%s", inputString);
								bb = max;
								while (bb > i+1 && bb > 0) {
									putc('\b', stdout);
									bb--;
								}
								max--;
								escapeFlg = 0;
								continue;
							} else if (inChar == 0x33) {
								escapeFlg = 4;  
								continue;
							} else {
								escapeFlg = 0;
								continue;
							}
						} else {
							escapeFlg = 0;
							continue;
						}
					} else if (inChar == 0x4f && escapeFlg == 1) {
						escapeFlg = 3;
						continue;
					} else if (escapeFlg == 3) {
						if (inChar == 0x48) {
							while (i) {
								putc(0x8, stdout);
								i--;
							}
							escapeFlg = 0;
							continue;
						} else if (inChar == 0x46) {
							while (i < max) {
								putc(*(inputString + i), stdout);
								i++;
							}
							escapeFlg = 0;
							continue;
						} else {
							escapeFlg = 0;
							continue;
						}
					}
				}
			} else if (inChar >= 0x20 && inChar <= 0x7e) { /* Used to be a normal input :) */
				if (i != max){
					bb = max;
					while (bb >= i) {
						*(inputString + bb + 1) = *(inputString + bb);
						bb--;
					}
					bb = 0;
					while (bb <= max) {
						putc(0x20, stdout);
						bb++;
					}
					*(inputString + i) = inChar;
					putc('\r', stdout);
					_lolish_line();
					printf("%s", inputString);
					fflush(stdout);
					while (bb > i+1) {
						putc('\b', stdout);
						bb--;
					}
				} else {
					*(inputString + i) = inChar;
					putc(inChar, stdout);
					fflush(stdout);
				}
				i++;
				max++;
			}
		}
		while (1);
	} 
	while (1);
	return 0;
}

