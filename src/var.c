/****            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE                 **
**                         Version 2, December 2004                          **
**         Copyright (C) 2013 Kristiyan Peychev <kurotsuki_l@yahoo.com>      **
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
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <termio.h>
#include <fcntl.h>
#include <dirent.h>
#include "varflag.h"
#include "sukebei_calc.h"
#include "header.h"

#define sizeOfAllocation	(256)


char	**varstorage, **varValues,
	*varIsOfType;

int	numAllocated = 0, starting = 0;

void	allocate(void){
	int i;
	varstorage = (char **) malloc(numAllocated * sizeof(char **));
	varIsOfType = (char *) malloc(numAllocated * sizeof(char *));
	varValues = (char **) malloc(numAllocated * sizeof(char **));
	for (i = 0; i < numAllocated; i++) {
		*(varstorage+i) = (char *) malloc(sizeOfAllocation);
		*(varValues+i) = (char *) malloc(sizeOfAllocation * 16);
	}
}

void	reAllocate(){
	int i;
	varstorage = (char **) realloc(varstorage, numAllocated * sizeof(char *));
	varIsOfType = (char *) realloc(varIsOfType, numAllocated * sizeof(char *));
	varValues = (char **) realloc(varValues, numAllocated * sizeof(char *));
	for (i = (numAllocated/2); i < numAllocated; i++) {
		*(varstorage+i) = (char *) malloc(sizeOfAllocation * sizeof(char));
		*(varValues+i) = (char *) malloc(sizeOfAllocation * 16 * sizeof(char));
	}
}

void	var(void *args){
	char varName [sizeOfAllocation] = "", buffer [sizeOfAllocation * 16] = "", *argv = (char *) args, buf [sizeOfAllocation] = "";
	int i = 0, j = 0, k = 0, flag = 0, argc = strlen(argv), f = 0, prevCh = 0;
	if (varstorage == NULL) {
		numAllocated = 8;
		allocate();
	}
	if (starting == (numAllocated-1)) {
		numAllocated *= 2;
		reAllocate();
	}
	while (*(argv+i) != 0x20 && *(argv+i) != 0x10 && *(argv+i) != 0x3d && *(argv+i) != 0x3b) {
		*(varName+i) = *(argv+i);
		i++;
	}
	while (j < starting) {
		if (!strcmp(varName, *(varstorage+j))) {
			free(*varValues+j);
			*(varValues+j) = (char *) malloc(sizeOfAllocation * 4);
			flag++;
			break;
		}
		j++;
	}
	if (flag) 
		fprintf(stdout, "lolish info: This way of changing a variable's value is not recomended for use. It may cause bugs.\n");
	else 
		strcpy(*(varstorage+starting), varName);
	while (*(argv+i)) {
		if (*(argv+i) == 0x20) {		// SPAAAAAACE  
		} else if (*(argv+i) == 0x3b) {		//  ';'
			flag |= semicolon;
			break;
		} else if (*(argv+i) == 0x10) 		// '\n'
			;
		else if (*(argv+i) == 0x3d) { 		// '='
			flag |= 1;
		} else if (*(argv+i) == 0x20 && prevCh != 0x5c || *(argv+i) == 0x21 || *(argv+i) > 0x22 && *(argv+i) < 0x27 || *(argv+i) > 0x29 && *(argv+i) <= 0x30 && *(argv+i) > 0x39 ) {
			fprintf(stdout, "lolish: Wrong variable declaration!\n");
			return;
		} else if (*(argv+i) == 0x22 || *(argv+i) == 0x27) {
			flag |= STRING;
			if (flag & INT || flag & _DIR) {
			} else {
				fprintf(stdout, "lolish: Wrong variable declaration!\n");
				return;
			}
		} else if (*(argv+i) >= 0x30 && *(argv+i) <= 0x39) {
			flag |= INT;
			if (!(flag & STRING || flag & _DIR)) {
				break;
			} else {
				fprintf(stdout, "lolish: Wrong variable declaration!\n");
				return;
			}
		} else if(*(argv+i) == 0x2f || *(argv+i) == 0x7e) {
			flag |= _DIR;
			if (!(flag & STRING || flag & INT)) {
				break;
			} else {
				fprintf(stdout, "lolish: Wrong variable declaration!\n");
			}
		} else if (*(argv+i) > 0x40 && *(argv+i) < 0x5b || *(argv+i) > 0x60 && *(argv+i) < 0x7b){
			if (flag & INT || flag & STRING || flag & _DIR) {
				break;
			} else {
				fprintf(stdout, "lolish: Wrong variable declaration!\n");
				return;
			}
		}
		prevCh = *(argv+i);
		i++;
	}
	flag ^= 1;
	if ((flag & semicolon)) {
		return;
	} 
	while (i < argc) {
		if (flag & STRING) {
			if (*(argv+i) == 0x22 && !f) {
				f = 1;
			} else if (*(argv+i) == 0x27 && !f) {
				f = 2;
			} else if (f == 1 && *(argv+i) == 0x27 || f == 2 && *(argv+i) == 0x22) {
				fprintf(stdout, "lolish: Wrong variable declaration... Tard!\n");
				return;
			} else if (i != (argc-1) && f &&((*(argv+i) == 0x22 && f == 1) || (*(argv+i) == 0x27 && f == 2))) {
				fprintf(stdout, "lolish: Wrong variable declaration!\n");
				return;
			} else {
				//	*(varIsOfType+starting) = STRING;
			}
		} else if (flag & INT) {
			if (*(argv+i) == 0x2e) {
				if (flag & FLOAT){
				       fprintf(stdout, "lolish: Wrong variable declaration!\n");
				       return;
				} else flag |= FLOAT;
			} else if ((flag & FLOAT) && (*(argv+i) < 0x30 || *(argv+i) > 0x39)) {
				fprintf(stdout, "lolish: Wrong variable declaration... Tard!\n");
				return;
			}
		}
		buf [k] = *(argv+i);
		i++;
		k++;
	}
	strcpy(*(varValues+starting), buf);
	*(varIsOfType+starting) = flag;
	starting++;
}

char  **cmdlist;
int	totwrit;

int	_lolish_refresh(void){
	if( (cmdlist = getCommands(&totwrit)) == NULL )
		return -1;
return 0;
}
char	getVarVal(char *argv, int looper){
	char buffer[256];
	int f = 0, d = 0;
	while (*(argv + looper) != 0x20 && *(argv + looper) != '\0') {
		*(buffer + f) = *(argv + looper);
		f++;
		looper++;
	}
	while (strcmp(buffer, *(varstorage))){
		d++;
		if (d > starting) {
			fprintf(stdout, "lolish: Local variable `%s` not found.\n", buffer);
			return -1;
		}
	}
	strcpy(argv, *(varValues + d));
return 0;
}

int	coreCMDCheck(char *this){
	short i = 0;
	static int c = 0;
/*	if (c == 0)		// You'll get this alot if you uncomment it, not recommented.
		if (_lolish_refresh() < 0) printf("BADBADBADBADBADBADBADBADBADBAD\n");
*/
	c++;
	while (i < totwrit) {
		if (!strcmp(this, *(cmdlist + i))) 
			return 0;
		i++;
	}
	printf("lolish: `%s' was not found.\n", this);
return -1;
}

char **mbuf;

int	_lolish_fdpopen(char *argv, int _stdin, int _stdout, int th){
	int	out_fds[2], in_fds[2], f = 0, k = 0, j = 0, i = 0, space = 0, size;
	pid_t	child_pid;
	char	buff/*, **mbuf*/; //Weird shit, moved upwards by a few lines (at top-level)
	while (*(argv + i)) {
		if (*(argv + i) == 0x20) 
			space++;
		i++;
	}
	size = i;
	space++;
	mbuf = (char **) malloc(space * sizeof(char *));
	if ((void *) mbuf == NULL) {
		fprintf(stdout, "lolish: Failed to allocate space\n");
		return 0777;
	}
	for (i = 0; i < space; i++) {
		*(mbuf + i) = (char *) malloc(64 * sizeof(char));
		if (*(mbuf + i) == NULL) {
			fprintf(stdout, "lolish: Failed to allocate space\n");
			return 0777;
		}
	}
	i = 0;
	while (1) {
		if (*(argv + i) == 0x20) {
			*(*(mbuf + j)+ k) = 0;
			i++;
			j++;
			k = 0;
			continue;
		} else if (*(argv + i) == 0) {
			*(*(mbuf + j)+ k) = 0;
			break;
		}
		*(*(mbuf + j)+ k) = *(argv + i);
		k++;
		i++;
	}
	if (!coreCMDCheck(*mbuf)){
	} else if (**mbuf != '/') {
		return -2;
		for (i = 0; i < space; i++) {
			free(*(mbuf+i));
		}
	}
	*(mbuf+(j+1)) = (char *) 0;
	if (pipe(in_fds) == -1 || pipe(out_fds) == -1) {
		fprintf(stdout, "lolish: Failed to execute\n");
		return -4;
	}
	child_pid = fork();
	if (child_pid == -1) {
		return -2;
	} else if (child_pid == 0) {
		close(out_fds[0]);
		close(in_fds[1]);
		signal(SIGINT, SIG_DFL);
		signal(SIGHUP, SIG_DFL);
		signal(SIGABRT, SIG_DFL);
		if (dup2(_stdin, STDIN_FILENO) < 0 || (_stdout != 0 ? dup2(_stdout, STDOUT_FILENO) : 1) < 0) 
			return -3;
		lseek(_stdin, 0, SEEK_SET);
		execvp(*mbuf, mbuf);
		close(out_fds[1]);
		while (read(in_fds[0], &buff, 1) > 0) 
			;
		close(in_fds[0]);
		_exit(EXIT_SUCCESS);
	} else {
		close(in_fds[0]);
		close(out_fds[1]);
		while (read(out_fds[0], &buff, 1) > 0) 
			;
		close(out_fds[0]);
		for (i = 1; i < space; i++)
			free(*(mbuf + i)); 
		close(in_fds[1]);
		(!th ? wait(NULL) : 0);
	}
return child_pid;
}

void	_lolish_tpl(char *tmpl){
	static char boosh = 'k', sh = 'N';
	char some_shit[] = ".XXXXXX";
	boosh++;
	sh++;
	strcpy(tmpl, "/tmp/");
	*(tmpl + 5) = boosh;
	*(tmpl + 6) = sh;
	*(tmpl + 7) = 0;
	strcat(tmpl, some_shit);
return;
}

void	exec (void *allz){
	char	*argv = (char *) allz, **threads, *special_buffer,
		template[512], buf;
	int 	i = 0, j = 0, k = 0, looper = 0, size, *th_size,
		pipe_count = 0, th_count = 0, *pipe_loc, *th_loc,
		space = 0, *in_red, *out_red, *err_red, p = 0, in_buf,
		out_buf, err_buf, th_cc = 0;
	pipe_loc = th_loc = NULL;
	special_buffer = NULL;
	size = strlen(argv);
	while (looper < size) {
		if ((*argv + looper) == '$') {
			getVarVal(argv, looper);
		} else if (*(argv + looper) == '|') {
			pipe_loc = (int *) realloc(pipe_loc, ((pipe_count+1) * sizeof(int)));
			*(pipe_loc + pipe_count++) = looper;
		} else if (*(argv + looper) == '&') {
			th_loc = (int *) realloc(th_loc, ((th_count+1) * sizeof(int)));
			*(th_loc + th_count++) = looper;
		}
		looper++;
	}
	th_size = (int *) malloc(th_count+1 * sizeof(int));
	threads = (char **) malloc(th_count+1 * sizeof(char *));
	for (i = 0; i < th_count+1; i++) {
		*(threads + i) = (char *) malloc(128 * sizeof(char));
	}
	i = 0;
	space = 0;
	while (i <= th_count) {
		k = 0;
		while (j < ( i == th_count ? size : th_loc[i])) {
			if (*(argv + j) == 0x20 && space == 0) 
				;
			else { 
				*(*(threads + i)+ k) = *(argv + j);
				space++;
				k++;
			}
			j++;
		}
		space = 0;
		*(*(threads + i) + k) = 0;
		*(th_size + i) = k;
		j++;
		i++;
	}
	if (th_count == 0) {
		if (pipe_count == 0) {
			_lolish_fdpopen(argv, 0, 0, 0);
		} else {
			k = 0;
			_lolish_tpl(template);
			in_buf = mkstemp(template);
			special_buffer = (char *) malloc(1024 * sizeof(char));
			while (1) {
				i = 0;
				memset(special_buffer, 0, *(th_size + i) + 1);
				_lolish_tpl(template);
				out_buf = mkstemp(template);
				if (out_buf < 0) {
					fprintf(stdout, "lolish: Oh the horror: Cannot create a TMP file, exiting cuz I can\n");
					exit(1);
				}
				while (1){
					if (*(argv + k) == '|' || *(argv + k) == 0) {
						i--;
						while (*(special_buffer + i) == 0x20) {
							*(special_buffer + i--) = 0;
						}
						break;
					} else if (*(argv + k) == 0x20 && space == 0) {
						k++;
						continue;
					}
					space++;
					*(special_buffer + i) = *(argv + k);
					k++;
					i++;
				}
				if (p == 0) {
					lseek(out_buf, 0, SEEK_SET);
					_lolish_fdpopen(special_buffer, 0, out_buf, 0);
				} else {
					lseek(in_buf, 0, SEEK_SET);
					_lolish_fdpopen(special_buffer, in_buf, out_buf, 0);
					close(in_buf);
					_lolish_tpl(template);
					in_buf = mkstemp(template);
				}
				k++;
				p++;
				lseek(out_buf, 0, SEEK_SET);
				while (read(out_buf, &buf, 1) > 0) {
					write(in_buf, &buf, 1);
				}
				close(out_buf);
				space = 0;
				if (*(argv + k) == 0) 
					break;
			}
			lseek(in_buf, 0, SEEK_SET);
			while (read(in_buf, &buf, 1) > 0) {
				putc(buf, stdout);
			}
		}
	} else if (pipe_count == 0) {										/* incomplete */
		while (th_cc < i) {
			if (!strcmp(*(threads + th_cc), "")) 
				break;
			_lolish_fdpopen(*(threads + th_cc), 0, 0, 1);
			th_cc++;
		}
	} else {
		k = j = 0;
		_lolish_tpl(template);
		in_buf = mkstemp(template);
		special_buffer = (char *) malloc(1024 * sizeof(char)); 
		while (i) {
			i--;
			_lolish_tpl(template);
			out_buf = mkstemp(template);
			if (out_buf < 0) {
				printf("HOLY FUCK!\n");
				exit(1);
			}
			memset(special_buffer, 0, 1024);
			while(k < *(th_size + i)){
				while (*(*(threads + i)+ k) != '|' && k < *(th_size + i)) {
					*(special_buffer + k) = *(*(threads + i)+ k);
					k++;
				}
				*(special_buffer + k) = 0;
				if (j == 0) {
					_lolish_fdpopen(special_buffer, 0, out_buf, 1);
				} else {
					_lolish_fdpopen(special_buffer, in_buf, out_buf, 1);
					close(in_buf);
					_lolish_tpl(template);
					in_buf = mkstemp(template);
				}
				lseek(out_buf, 0, SEEK_SET);
				while (read(out_buf, &buf, 1) > 0) {
					write(in_buf, &buf, 1);
				}
				j++;
			}
			free(special_buffer);
		}
	}
	for (i = 0; i < th_count + 1; i++) 
		free(*(threads + i));
	free(threads);
	free(th_size);
	free(pipe_loc);
	free(th_loc);
return;
}

