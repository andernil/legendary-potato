#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#define _GNU_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>


FILE* driver;


void sigio_handler(int signo)
{
    printf("knapp: %d\n",fgetc(driver));
}

int main(int argc, char *argv[])
{
	printf("Game starting\n");
	
	driver = fopen("/dev/driver-gamepad", "rb");

	if (driver == NULL) {
		printf("Driver error\n");
		exit(EXIT_SUCCESS);
	}


	signal(SIGIO, &sigio_handler);
	fcntl(fileno(driver), F_SETOWN, getpid());

	long oflags = fcntl(fileno(driver), F_GETFL);
    	fcntl(fileno(driver), F_SETFL, oflags | FASYNC);
	
	while(1){
	    printf("\n");
	    pause();
	};

	exit(EXIT_SUCCESS);
}
