#include<iostream>
#include<./sys/types.h>
#include<./sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<./arpa/inet.h>
#include<string.h>
#include<string>

int main() {

    //Create a socket
    int sock1 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock1 == -1) {
        std::wcerr << "Cant Create a Socket";
        return -1;
    }
    //Bind the socket to Listening port
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(53000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);


    if (bind(sock1, (sockaddr*)&hint, sizeof(hint)) == -1) 
    {
        std::wcerr << "Cant Bind to IP/port";
        return -2;
    }
    // Change the socket to listening now that is binded
    if (listen(sock1, SOMAXCONN) == -1) {

        std::wcerr << "Cant Listen!";
        return -3;
    }

    // Accept a Call
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSocket = accept(sock1, (sockaddr*)&client, &clientSize);

    if (clientSocket == 1) {
        std::wcerr << "Problem with client connecting";
        return -4;
    }

    // Close the listening socket
    close(sock1);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

    if (result) {
        std::cout << host << " connected on " << svc << std::endl;
    }
    else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }

    char buf[4096];
    while (true)
    {
        // Clear the Buffer
        memset(buf, 0, 4096);
        // Wait for a message
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if (bytesRecv == -1) {
            std::wcerr << "There was a connection issue pls contact administration" << std::endl;
            break;
        } 
        if (bytesRecv == 0) {
            std::cout << "The client Disconnected" << std::endl;
            break;
        }
        
        // Display message
        std::cout << "Recieved: " << std::string(buf, 0, bytesRecv) << std::endl;
        
        // Resend message
        send(clientSocket, buf, bytesRecv +1, 0);
    }
    // Close the Socket
    close(clientSocket);
    return 0;
}