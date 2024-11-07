
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 500
#define PORT 8080

// handles client messages/ bytes sent
void handle_client(int client_sock){
    char buffer[BUF_SIZE];
    ssize_t nread;
    while (1){
	nread = read(client_sock, buffer, BUF_SIZE);

	if (nread == -1){
	    perror("read");
	    close(client_sock);
	    exit(EXIT_FAILURE);
	} else if (nread == 0){
	    printf("Client disconnected\n");
	    break;
	}

	buffer[nread] = '\0';
	printf("Recvieved %zd bytes: %s\n", nread, buffer);

	if(write(client_sock, buffer, nread) != nread){
	    perror("write");
	    close(client_sock);
	    exit(EXIT_FAILURE);
	}
    }

    close(client_sock);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{

   int                      sfd, s, client_sock;
   char                     buf[BUF_SIZE];
   ssize_t                  nread, read_client;
   socklen_t                peer_addrlen;
   struct addrinfo          hints;
   struct addrinfo          *result, *rp;
   struct sockaddr_storage  peer_addr;
   peer_addrlen = sizeof(peer_addr);

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
   hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
   hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
   hints.ai_protocol = 0;          /* Any protocol */
   hints.ai_canonname = NULL;
   hints.ai_addr = NULL;
   hints.ai_next = NULL;

   s = getaddrinfo(NULL, "8080", &hints, &result);
   if (s != 0) {
       fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
       exit(EXIT_FAILURE);
   }

   /* getaddrinfo() returns a list of address structures.
      Try each address until we successfully bind(2).
      If socket(2) (or bind(2)) fails, we (close the socket
      and) try the next address. */


   for (rp = result; rp != NULL; rp = rp->ai_next) {
       sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
       if (sfd == -1)
	   continue;

       if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
	   break;                  /* Success */

       close(sfd);
   }

    printf("Listening on port %d...\n", PORT);
   freeaddrinfo(result);           /* No longer needed */
    
    if (listen(sfd, SOMAXCONN) == -1){
	perror("listen");
	close(sfd);
	exit(EXIT_FAILURE);
    }

   if (rp == NULL) {               /* No address succeeded */
       fprintf(stderr, "Could not bind\n");
       exit(EXIT_FAILURE);
   }

    while (1){
	client_sock = accept(sfd, (struct sockaddr *)&peer_addr, &peer_addrlen);

	if(client_sock < 0 ){
	    perror("accept");
	    continue;
	}

	pid_t pid = fork();
	if (pid < 0){
	    perror("fork");
	    close(client_sock);
	    continue;
	} else if (pid == 0){
	    close(sfd);
	    handle_client(client_sock);
	} else{
	    close(client_sock);
	}
    }

    close(sfd);
    return 0;
}
