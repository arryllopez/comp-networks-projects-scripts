/*
 * hello_client.c
 * SOFE 3850U - Computer Networks
 * Part 2-III: Hello Application (TCP)
 *
 * Modified from echo_client.c
 * The client connects to the server, waits for the "Hello" message,
 * displays it on the terminal, and exits when the server closes
 * the TCP connection (read returns 0).
 *
 * Usage: ./hello_client <server_IP> <port_number>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFLEN 256

int main(int argc, char *argv[])
{
    int sd, port;
    ssize_t bytes_read;
    char *bp, rbuf[BUFLEN];
    struct sockaddr_in server;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_IP> <port_number>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[2]);

    /*
     * Create a TCP socket.
     * sd = socket(AF_INET, SOCK_STREAM, 0);
     */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("socket");
        exit(1);
    }

    /*
     * Push the server address information (server IP address and
     * port number) into the structure variable "server".
     */
    memset(&server, 0, sizeof(server));
    server.sin_family      = AF_INET;
    server.sin_port        = htons(port);
    server.sin_addr.s_addr = inet_addr(argv[1]);

    /*
     * Call connect to make a TCP connection with the server.
     * If connect returns with a non-negative number, a TCP
     * connection has been established with the server.
     * connect(sd, (struct sockaddr *)&server, sizeof(server));
     */
    int connect_result = connect(sd, (struct sockaddr *)&server, sizeof(server));
    if (connect_result == -1) {
        perror("connect");
        close(sd);
        exit(1);
    }
    printf("Connected to server %s:%d\n", argv[1], port);

    /*
     * In echo_client, the client reads from the terminal, sends to
     * the server, then reads the echo back:
     *
     *   printf("Transmit: \n");
     *   while(n=read(0, sbuf, BUFLEN)){
     *       write(sd, sbuf, n);
     *       printf("Receive: \n");
     *       bp = rbuf;
     *       bytes_to_read = n;
     *       while ((i = read(sd, bp, bytes_to_read)) > 0){
     *           bp += i;
     *           bytes_to_read -= i;
     *       }
     *       write(1, rbuf, n);
     *       printf("Transmit: \n");
     *   }
     *
     * In the hello_client, the client does not send anything.
     * It only waits to receive the Hello message from the server.
     * Since TCP may not deliver the message in one packet, we keep
     * reading until read returns 0, which indicates the server has
     * closed the TCP connection (as described in Figure 4).
     */
    printf("Waiting for message...\n");

    bp = rbuf;
    while ((bytes_read = read(sd, bp, BUFLEN - (bp - rbuf))) > 0) {
        bp += bytes_read;
    }
    if (bytes_read == -1) {
        perror("read");
        close(sd);
        exit(1);
    }

    /*
     * read returns 0 when the server closes the TCP connection.
     * Display the received message on the terminal.
     * write(1, ...) writes to standard output (the terminal),
     * same as in echo_client.
     */
    printf("Receive:\n");
    write(1, rbuf, bp - rbuf);
    printf("\n");

    /*
     * The TCP connection has been terminated by the server.
     * Close the socket and exit.
     */
    close(sd);
    printf("Server closed the connection. Client exiting.\n");

    return 0;
}
