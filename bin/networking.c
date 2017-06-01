//
// Created by vetrix on 23/05/17.
//

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <signal.h>
#include <unistd.h>
#include "files.h"
#include "io.h"
#include<stdlib.h>    //strlen
#include<pthread.h> //for threading , link with lpthread
#include <netinet/in.h>
#include <sys/types.h>

void *client_handler(void *);

int server_handler(int port , char *User, char *IPaddress )
{
    int socket_desc , client_sock , c , *n_sockfd;
    struct sockaddr_in server , client;

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    //server.sin_addr.s_addr = inet_addr( settings.IPAddress ) ;
    server.sin_port = htons( 9001 );

    //testing purposes
    server.sin_addr.s_addr = INADDR_ANY;

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;

    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);


    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");

        pthread_t thread_listen;
        n_sockfd = malloc(1);
        *n_sockfd = client_sock;

        if( pthread_create( &thread_listen , NULL ,  client_handler , (void*) n_sockfd) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }

    return 0;
}

/*
 * This will handle connection for each client
 * */
void *client_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];

    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));

    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));

    //Receive a message from client | Receive the input given from client
    //Use this for reading a response to download a file/sharelist/friendslist
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        write(sock , client_message , strlen(client_message));
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(socket_desc);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}






























//
//void add_file()
//{
//    if ( (!strcmp(username, perm)) || (!strcmp(perm,"All")) )       // If the User matches the permission or the permission is all
//    {
//        printf("\n%s found, sending to client...\n", f_name);
//        send(sock, f_name, sizeof(f_name), 0);
//        found = TRUE;
//
//        fd = open(direct, O_RDONLY);
//        printf("%s", direct);
//
//        if (fd != 0)
//        {
//            perror("Unable to open file\n");
//            return(1);
//        }
//
//        fstat(fd, &stat_buff);
//        offset = 0;
//        rc = sendfile(sock, fd, &offset, stat_buff.st_size);
//        if (rc == 1)
//        {
//            perror("Error from sendfile");
//            return(1);
//        }
//        if (rc != stat_buff.st_size)
//        {
//            fprintf(stderr, "incomplete transfer from sendfile: %d of %d bytes\n", rc, (int)stat_buff.st_size);
//            return(1);
//        }
//        snprintf(text_send, 255, "%s downloaded from server %s\n", f_name, username);
//        send(sock, text_send, sizeof(text_send) , 0);
//    }
//}
