#include <stdio.h>        // Includes Standard Input/Output functions
#include <stdlib.h>       // Includes Standard Library functions
#include <string.h>       // Includes String handling functions   
#include <unistd.h>       // Provides access to POSIX OS API
#include <netinet/in.h>   // Contains constants and structures for internet domain addresses

#define PORT 80         // Default HTTP port 
int main() {
    int server_fd, new_socket;    
    struct sockaddr_in address;   // Structure containing an internet address
    int addrlen = sizeof(address); // Size of the address structure (16 bytes)
    int opt = 1;

    char buffer[3000] = {0};

    // Creating the socket with IPv4 (AF_INET) and the SOCK_STREAM is the TCP protocol 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed"); 
        exit(EXIT_FAILURE);  
    }

    // Set socket options to allow reuse of local addresses and ports
    // the opt is to enable options for the setsockopt that's why it is also set to 1
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt))) {
        perror("setsockopt");
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
        char httpResponse[4096];
        printf("====== Waiting for new connection ======\n");
        char buffer[3000] = {0}; 

        // Accept a connection,the function accept returns a new file description that's we use a new socket for it
        // We check < 0 because the accept functions returns -1 if there is any error
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");   
            exit(EXIT_FAILURE);  
        }

        // Read message from client
        int bytesRead = read(new_socket, buffer, sizeof(buffer) - 1);  // Read data into buffer from the new socket
        if (bytesRead < 0) {
            perror("read");
            close(new_socket);
            continue;
        }

        // Null-terminate the buffer to make it a valid string
        buffer[bytesRead] = '\0';

        // Parse the HTTP method, path, and version from the request
        char method[10], path[1024], httpVersion[9];
        int sscanfResult = sscanf(buffer, "%s %s %s", method, path, httpVersion); // Using sscanf to read the method, path, and HTTP version
        if (sscanfResult != 3) {
            // Handle error or unsupported request format
            printf("Failed to parse request. Only received %d tokens.\n", sscanfResult);
            close(new_socket);
            continue;
    }
        printf("HTTP Method: %s\n", method);
        printf("HTTP Path: %s\n", path);

        // Ensure that the HTTP method and path are null-terminated
        method[sizeof(method) - 1] = '\0';
        path[sizeof(path) - 1] = '\0';

        // Create the HTTP response with the path included in the body
        sprintf(httpResponse, 
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/html\n"
            "Content-Length: %lu\n\n" // The content length should be calculated based on the body content
            "<h1>Hello from %s </h1>", // Include the path in the response
            strlen(path) + 16, // The content length, 16 is the length of "<h1>Hello from </h1>"
            path);

        // Response to the socket with the httpResponse and we send the string lenght to make sure the response is send correctly
        write(new_socket, httpResponse, strlen(httpResponse));

        close(new_socket); // Close the client socket to optimize resources
    }

    // Close the socket
    close(server_fd); // Close the listening socket

    return 0; // Exit program with success status
}
