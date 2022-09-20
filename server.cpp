
//#include <iostream>
//#include <string.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <stdlib.h>
//#include <unistd.h>
////#include <sys/time.h>
//
//using namespace std;
//
//int main()
//{
//    /* ---------- INITIALIZING VARIABLES ---------- */
//
//    int client, server;
//    int portNum = 1500;
//    bool isExit = false;
//    int bufsize = 1024;
//    char buffer[bufsize];
//
//    struct sockaddr_in server_addr;
//    socklen_t size;
//
//    /* ---------- ESTABLISHING SOCKET CONNECTION ----------*/
//    /* --------------- socket() function ------------------*/
//
//    client = socket(AF_INET, SOCK_STREAM, 0);
//
//    if (client < 0) 
//    {
//        cout << "\nError establishing socket..." << endl;
//        exit(1);
//    }
//
//    cout << "\n=> Socket server has been created..." << endl;
//
//    server_addr.sin_family = AF_INET;
//    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
//    server_addr.sin_port = htons(portNum);
//
//    /* ---------- BINDING THE SOCKET ---------- */
//    /* ---------------- bind() ---------------- */
//
//
//    if ((bind(client, (struct sockaddr*)&server_addr,sizeof(server_addr))) < 0) 
//    {
//        cout << "=> Error binding connection, the socket has already been established..." << endl;
//        return -1;
//    }
//
//    size = sizeof(server_addr);
//    cout << "=> Looking for clients..." << endl;
//
//    /* ------------- LISTENING CALL ------------- */
//    /* ---------------- listen() ---------------- */
//
//    listen(client, 1);
//
//    /* ------------- ACCEPTING CLIENTS  ------------- */
//    /* ----------------- listen() ------------------- */
//
//    int clientCount = 1;
//    server = accept(client,(struct sockaddr *)&server_addr,&size);
//
//    // first check if it is valid or not
//    if (server < 0) 
//        cout << "=> Error on accepting..." << endl;
//
//    while (server > 0) 
//    {
//        strcpy(buffer, "=> Server connected...\n");
//        send(server, buffer, bufsize, 0);
//        cout << "=> Connected with the client #" << clientCount << ", you are good to go..." << endl;
//        cout << "\n=> Enter # to end the connection\n" << endl;
//
//        cout << "Client: ";
//        do {
//            recv(server, buffer, bufsize, 0);
//            cout << buffer << " ";
//            if (*buffer == '#') {
//                *buffer = '*';
//                isExit = true;
//            }
//        } while (*buffer != '*');
//
//        do {
//            cout << "\nServer: ";
//            do {
//                cin >> buffer;
//                send(server, buffer, bufsize, 0);
//                if (*buffer == '#') {
//                    send(server, buffer, bufsize, 0);
//                    *buffer = '*';
//                    isExit = true;
//                }
//            } while (*buffer != '*');
//
//            cout << "Client: ";
//            do {
//                recv(server, buffer, bufsize, 0);
//                cout << buffer << " ";
//                if (*buffer == '#') {
//                    *buffer = '*';
//                    isExit = true;
//                }
//            } while (*buffer != '*');
//        } while (!isExit);
//
//        /* ---------------- CLOSE CALL ------------- */
//        /* ----------------- close() --------------- */
//
//        // inet_ntoa converts packet data to IP, which was taken from client
//        cout << "\n\n=> Connection terminated with IP " << inet_ntoa(server_addr.sin_addr);
//        close(server);
//        cout << "\nGoodbye..." << endl;
//        isExit = false;
//        exit(1);
//    }
//
//    close(client);
//    return 0;
//}


#include <stdio.h>
#include <iostream>
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 

#include <sys/types.h>
#define _XOPEN_SOURCE_EXTENDED 1
#define _OE_SOCKETS
#define _USE_GNU
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>    //close 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
     
#define TRUE   1 
#define FALSE  0 
#define PORT 8888 
     
int main(int argc , char *argv[])  
{  
    (void)argc;
    (void)argv;
    int opt = TRUE; 
    bool isExit = false;
    int master_socket , addrlen , new_socket , client_socket[30] , 
          max_clients = 30 , activity, i , valread , sd;  
    int max_sd;  
    struct sockaddr_in address;  
    int bufsize = 1024;
    char buffer[1025];  //data buffer of 1K 
         
    //set of socket descriptors 
    fd_set readfds;  
         
    //a message 
    char message[] = "ECHO Daemon v1.0 \r\n";  
     
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
         
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
     
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
     
    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
         
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    } 
    //    /* ------------- LISTENING CALL ------------- */
    //    /* ---------------- listen() ---------------- */
    printf("Listener on port %d \n", PORT);  
         
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
         
    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  
         
    while(TRUE)  
    {  
        //clear the socket set 
        FD_ZERO(&readfds);  
     
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
             
        //add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            sd = client_socket[i];  
                 
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                 
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
     
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
             
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  
        {  
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
             //
            //inform user of socket number - used in send and receive commands 
            std::cout << "New connection , socket fd is " << new_socket << ", ip is : "<< inet_ntoa(address.sin_addr) << " , port : "<< ntohs(address.sin_port) << std::endl;
           
            //send new connection greeting message 
            if( send(new_socket, message, strlen(message), 0) != (ssize_t)strlen(message))  
            {  
                perror("send");  
            }  
                 
            puts("Welcome message sent successfully");  
                 
            //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    std::cout << "Adding to list of sockets as " << i << std::endl;  
                    break;  
                }  
            }  
        }  
             
        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  
                 
            if (FD_ISSET( sd , &readfds))  
            {  
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , buffer, bufsize)) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
                    std::cout << "Host disconnected , ip " << inet_ntoa(address.sin_addr) << " , port " << ntohs(address.sin_port) << std::endl;  
                         
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0;  
                }  
                     
                //Echo back the message that came in 
                else 
                {  
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    buffer[valread] = '\0';
                    while (sd > 0) 
                    {
                        strcpy(buffer, "=> sd connected...\n");
                        send(sd, buffer, bufsize, 0);
                        std::cout << "=> Connected with the client #" << i << ", you are good to go..." << std::endl;
                        std::cout << "\n=> Enter # to end the connection\n" << std::endl;
                        std::cout << "Client: ";
                        do 
                        {
                            recv(sd, buffer, bufsize, 0);
                            std::cout << buffer << " ";
                            if (*buffer == '#') 
                            {
                                *buffer = '*';
                                isExit = true;
                            }
                        }
                        while (*buffer != '*');
                        do 
                        {
                            std::cout << "Client: ";
                            do 
                            {
                                std::cin >> buffer;
                                send(sd, buffer, bufsize, 0);
                                if (*buffer == '#')
                                {
                                    send(sd , buffer , bufsize , 0);
                                    *buffer = '*';
                                    isExit = true;
                                }
                            }
                            while (*buffer != '*');

                            std::cout << "Client: ";
                            do 
                            {
                                recv(sd, buffer, bufsize, 0);
                                std::cout << buffer << " ";
                                if (*buffer == '#')
                                {
                                    *buffer = '*';
                                    isExit = true;
                                }
                            } 
                            while (*buffer != '*');
                            /* ---------------- CLOSE CALL ------------- */
                            /* ----------------- close() --------------- */
                            // inet_ntoa converts packet data to IP, which was taken from client
                            std::wcout << "\n\n=> Connection terminated with IP " << inet_ntoa(address.sin_addr);
                            close(sd);
                            std::wcout << "\nGoodbye..." << std::endl;
                            isExit = false;
                            exit(1);
                        }
                        while (sd);
                            close(sd);
                        return (0);
                        //do 
                        //{
                        //    close(sd);
                        //    return (0); 
                        //}
                        
                    }
                }  
            }  
        }  
    }  
         
    return 0;  
} 

//ligne 307
//    while (sd > 0) 
//    {
//        strcpy(buffer, "=> sd connected...\n");
//        send(sd, buffer, bufsize, 0);
//        cout << "=> Connected with the client #" << clientCount << ", you are good to go..." << endl;
//        cout << "\n=> Enter # to end the connection\n" << endl;
//
//        cout << "Client: ";
//        do {
//            recv(sd, buffer, bufsize, 0);
//            cout << buffer << " ";
//            if (*buffer == '#') {
//                *buffer = '*';
//                isExit = true;
//            }
//        } while (*buffer != '*');
//
//        do {
//            cout << "\n sd: ";
//            do {
//                cin >> buffer;
//                send(sd, buffer, bufsize, 0);
//                if (*buffer == '#') {
//                    send(sd, buffer, bufsize, 0);
//                    *buffer = '*';
//                    isExit = true;
//                }
//            } while (*buffer != '*');
//
//            cout << "Client: ";
//            do {
//                recv(sd, buffer, bufsize, 0);
//                cout << buffer << " ";
//                if (*buffer == '#') {
//                    *buffer = '*';
//                    isExit = true;
//                }
//            } while (*buffer != '*');
//        } while (!isExit);
//
//        /* ---------------- CLOSE CALL ------------- */
//        /* ----------------- close() --------------- */
//
//        // inet_ntoa converts packet data to IP, which was taken from client
//        cout << "\n\n=> Connection terminated with IP " << inet_ntoa(sd_addr.sin_addr);
//        close(sd);
//        cout << "\nGoodbye..." << endl;
//        isExit = false;
//        exit(1);
//    }
//
//    close(client);
//    return 0;
