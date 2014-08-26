CC = gcc
CFLAGS = -lm


default: commandDIRs.o var.o files.o main.o
	$(CC) $(CFLAGS) commandDIRs.o var.o files.o main.o -o lolish
commandDIRs.o:
	$(CC) $(CFLAGS) -c src/commandDIRs.c
var.o:
	$(CC) $(CFLAGS) -c src/var.c
files.o:
	$(CC) $(CFLAGS) -c src/files.c 
main.o:
	$(CC) $(CFLAGS) -c src/main.c
install: default
	cp ./lolish /usr/bin
clean.o:
	rm ./*.o
clean: clean.o
	rm ./lolish*
