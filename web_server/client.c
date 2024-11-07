#include <sys/types.h>
#include <sys/socket.h> 
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <pthread.h>


#define BUF_SIZE 500

// function that will create threads for each new client that tries to access server

int main(int argc, char *argv[]) {

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, j;

    size_t len;
    ssize_t nread;
    char buf[BUF_SIZE];


    if (argc < 3) {
        fprintf(stderr, "Usage: %s host port msg...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Obtain address(es) matching host/port */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */


    s = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.

       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket
       and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                     rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  /* Success */

        close(sfd);
    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);           /* No longer needed */

    /* Send remaining command-line arguments as separate
       datagrams, and read responses from server */
    
    while (1){
        printf("Enter message (exit to quit): ");
        fgets(buf, BUF_SIZE, stdin);

        buf[strcspn(buf, "\n")] = '\0';

        if(strcmp(buf, "exit") == 0){
            printf("Exiting...\n");
            break;
        }

        if (write(sfd, buf, strlen(buf)) == -1){
            perror("write");
            close(sfd);
            exit(EXIT_FAILURE);
        }

        nread = read(sfd, buf, BUF_SIZE);
        if (nread == - 1){
            perror("read");
            close(sfd);
            exit(EXIT_FAILURE);
        }

        buf[nread] = '\0';
        printf("Server reply: %s\n", buf);
    }

    for (j = 3; j < argc; j++) {
        len = strlen(argv[j]) + 1;
                /* +1 for terminating null byte */

        if (len + 1 > BUF_SIZE) {
            fprintf(stderr,
                    "Ignoring long message in argument %d\n", j);
            continue;
        }

        if (write(sfd, argv[j], len) != len) {

            fprintf(stderr, "partial/failed write\n");
            exit(EXIT_FAILURE);
        }

        nread = read(sfd, buf, BUF_SIZE);
        if (nread == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("Received %zd bytes: %s\n", nread, buf);
    }

    close(sfd);
    exit(EXIT_SUCCESS); 
} // main
