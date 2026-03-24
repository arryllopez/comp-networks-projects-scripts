/*
 * hello_server.c, this is the sender of the message
 * SOFE 3850U - Computer Networks
 * Part 2-III: Hello Application (TCP)
 *
 * Modified from echo_server.c
 * After a TCP connection is established, the server sends
 * a "Hello" message to the client, closes the connection, and exits.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFLEN 256

int hellod(int sd);

int main(int argc, char *argv[])
{
    int sd, new_sd, port; // declaring socket descriptors and the port number it will be listening on
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port_number>\n", argv[0]);
        exit(1);
    }
    
    // converts the input of the port number from a string to an integer and stores it in the variable port
    port = atoi(argv[1]);

    /*
     * Create a TCP socket, with AF_INET for IPv4 and SOCK_STREAM for TCP, and 0 for the protocol. 
     */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) { // failure returns -1 
        perror("socket");
        exit(1);
    }

    /*
     * Assign IP address and port number to the opened socket.
     * bind(sd, (struct sockaddr *)&server, sizeof(server));
     */
    memset(&server, 0, sizeof(server));
    server.sin_family      = AF_INET;
    server.sin_port        = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    // execute the bind call to bind the socket to the specified address
    int bind_result = bind(sd, (struct sockaddr *)&server, sizeof(server));
    if (bind_result == -1) {
        perror("bind");
        close(sd);
        exit(1);
    }

    /*
     * Place the socket in passive (server) mode and tell the
     * TCP module to enqueue connection requests with max queue size 5.
     * listen(sd, 5);
     */
    int listen_result = listen(sd, 5);
    if (listen_result == -1) {
        perror("listen");
        close(sd);
        exit(1);
    }
    printf("Hello server listening on port %d...\n", port);

    /*
     * Accept an incoming TCP connection request.
     * accept() blocks until a connection request from a client arrives.
     * When it arrives, accept returns with a new socket descriptor (new_sd)
     * which can be used to communicate with the client.
     * new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
     */
    new_sd = accept(sd, (struct sockaddr *)&client, &client_len);
    if (new_sd == -1) {
        perror("accept");
        close(sd);
        exit(1);
    }
    printf("Client connected.\n");

    /*
     * In echo_server, the return of accept indicates a TCP connection
     * has been established. The echo_server then forks:
     *
     *   switch (fork()){
     *       case 0:
     *           (void) close(sd);
     *           exit(echod(new_sd));
     *       default:
     *           (void) close(new_sd);
     *           break;
     *       case -1:
     *           fprintf(stderr, "fork: error\n");
     *   }
    */ 

    (void) close(sd);
    exit(hellod(new_sd));
}

/*
 * hellod() - sends a "Hello" message to the client.
 * hellod reads from the client and writes (echoes) back in a loop.
 * hellod instead writes the Hello message to the client, then closes.
 */
int hellod(int sd)
{
    char buf[BUFLEN];
    int n;

    /* Prepare the Hello message in the buffer */
    strcpy(buf, "Hello");
    n = strlen(buf);

    /*
     * Send the Hello message to the client through the TCP socket.
     * The write system call sends the message; protocols at lower
     * layers (TCP, IP, Ethernet) take care of delivery.
     */
    ssize_t write_result = write(sd, buf, n);
    // error handling for write system call
    if (write_result == -1) {
        perror("write");
        close(sd);
        exit(1);
    }
    printf("Sent: %s\n", buf);

    /*
     * Close the socket. Since this is the last application attached
     * to the socket, closing it triggers the termination of the
     * TCP connection (FIN exchange as shown in Figure 4).
     */
    close(sd);
    printf("Connection closed. Server exiting.\n");
    return(0);
}
