#include <stdio.h>        // Includes Standard Input/Output functions
#include <stdlib.h>       // Includes Standard Library functions
#include <string.h>       // Includes String handling functions
#include <unistd.h>       // Provides access to POSIX OS API
#include <arpa/inet.h>    // Contains functions for internet operations

#define PORT 8080 // Setup 

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *message = "Hello from client";
    
    // Creating the socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    // Setting up the address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Send the message
    send(sock, message, strlen(message), 0);
    printf("Message sent\n");

    // Close the socket
    close(sock);

    return 0;
}
