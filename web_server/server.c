#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUF_SIZE 500
#define PORT 8080

// defining variables for the threads/ semaphores
sem_t x, y;
pthread_t tid;
pthread_t readerthreads[100];
int readcounter = 0;

// reader function
void* reader(void* param){
    // lock semaphore
    sem_wait(&x);
    readcounter++;

    // if readercount is not 0, it waits until the other semaphore unlocks
    if(readcounter == 1){
	sem_wait(&y);
    }

    // unlock sempahore
    sem_post(&x);

    printf("\nreader %d is inside.\n", readcounter);
    sleep(5);

    sem_wait(&x);
    readcounter--;

    if(readcounter == 0){
	sem_post(&y);
    }

    // lock x
    sem_post(&x);

    printf("%d reader is leaving\n", readcounter + 1);
    pthread_exit(NULL);
}

// handles client messages/ bytes sent
void* handle_client(int client_sock){
    char buffer[BUF_SIZE];
    ssize_t nread;
    
    sem_wait(&x);

    if (readcounter == 1){
	sem_wait(&y);
    }

    sem_post(&x);

    printf("\nreader %d is inside.\n", readcounter);
    sleep(5);

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
    sem_wait(&x);
    readcounter--;

    if(readcounter == 0){
	sem_post(&y);
    }

    // lock x
    sem_post(&x);

    printf("%d reader is leaving\n", readcounter + 1);
    pthread_exit(NULL);

    close(client_sock);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{

   int                      sfd, s, client_sock, i = 0;
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
	
	// creating a thread
	int choice = 0;
	recv(client_sock, &choice, sizeof(choice), 0);
	
	if (choice == 1){
	    if (pthread_create(&readerthreads[i++], NULL, reader, &choice) != 0)

	    printf("failed to create thread");
	}
    }


    close(sfd);
    return 0;
}
