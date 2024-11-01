#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 500

int main(int argc, char *argv[]){
	struct addrinfo hints;
	struct addrinfo *results, *rp;
	int sfd, s;
	size_t len;
	ssize_t nread;
	char buf[BUF_SIZE];

	if (argc < 3){
		fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
		exit(EXIT_FAILURE);
	}


}
