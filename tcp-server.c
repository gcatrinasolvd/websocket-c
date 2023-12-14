#include <stdio.h>        // Includes Standard Input/Output functions
#include <stdlib.h>       // Includes Standard Library functions
#include <string.h>       // Includes String handling functions   
#include <unistd.h>       // Provides access to POSIX OS API
#include <netinet/in.h>   // Contains constants and structures for internet domain addresses

#define PORT 8080         // Defines the port number on which the server will listen

int main() {
    int server_fd, new_socket;    
    struct sockaddr_in address;   // Structure containing an internet address
    int addrlen = sizeof(address); // Size of the address structure (16 bytes)

    // Creating the socket with IPv4 (AF_INET) and the SOCK_STREAM is the TCP protocol 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed"); 
        exit(EXIT_FAILURE);  
    }
    
    // Setting up the address structure
    address.sin_family = AF_INET;         // Address family (IPv4) We could use (IPv6)
    address.sin_addr.s_addr = INADDR_ANY; // IP address (accept connections on any IP)
    address.sin_port = htons(PORT);       // Port number (converts to network byte order)

    // Bind the socket to the address previously created, the server_fd (file description) is the identifier for the OS
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed"); 
        exit(EXIT_FAILURE);  
    }

    // Listen for connections. The 3 specifies the maximum number of pending connections in the socket queue
    // We check < 0 because the listen functions returns -1 if there is any error
    if (listen(server_fd, 3) < 0) {
        perror("listen");  
        exit(EXIT_FAILURE);  
    }

     while(1) { // Infinite loop for accepting connections
         //The size of the buffer is a standard of 1024 bytes (for simplicity and memory efficiency)
        char buffer[1024] = {0}; // Buffer for storing incoming messages

        // Accept a connection,the function accept returns a new file description that's we use a new socket for it
        // We check < 0 because the accept functions returns -1 if there is any error
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");   
            exit(EXIT_FAILURE);  
        }

        // Read message from client
        read(new_socket, buffer, 1024);  // Read data into buffer from the new socket
        printf("Message received: %s\n", buffer); 

        close(new_socket); // Close the client socket to optimize resources
    }

    // Close the socket
    close(server_fd); // Close the listening socket

    return 0; // Exit program with success status
}
