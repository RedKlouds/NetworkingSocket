/////////////////////////
//Danny Ly
//Filename: Server.cpp
// Description: Server side
//
//
//
//
//
/////////////////////////
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/uio.h>
#include <netinet/tcp.h>
#include <fcntl.h>

#include <signal.h>
#include <stdio.h>
#include <cstdio>
#include <ctime>
using namespace std;
//////
// Description: called on I/O interrupt
//
//////

const int BUFFSIZE = 1500;

void work(int signal_identifier){
    //Signal_identifier/SIGIO

    //allocate databuffer
    char databuff[BUFFSIZE];

    
}
int main(int argc, char *argv[]){
    clock_t start;
    start = clock();
    cout << "starting timer: " << start << endl;

    //Input argmuents that are expected
    //argv[1] Server IP port
    //argv[2] repetition: number of sending set of data buffers
    
    //TODO: Error checking

    int port_num = 22588;
    struct sockaddr_in acceptSockAddr;
    //zero initalize the struct
    bzero( (char*)&acceptSockAddr, sizeof( acceptSockAddr ) );
    //Address Family Internet
    acceptSockAddr.sin_family = AF_INET; 
    acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    acceptSockAddr.sin_port = htons( port_num);
    
    //server Descriptor
    int serverSd = socket( AF_INET, SOCK_STREAM, 0);

    //option to prompt OS to release the server port as soon as server process
    // is terminated
    const int on = 1; //flag variable to show port is in use

    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof( int ));

    //bind socket to local address, giving socket descriptr, socked address,
    //data size

    bind( serverSd, ( sockaddr* )&acceptSockAddr, sizeof( acceptSockAddr ));

    //inform OS to listen up to 5 connection requests from client at a time
    listen(serverSd, 5 );


    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof( newSockAddr );
    //Accept system call causes the process to block until a client connects to
    //the server, it will wake up the process when a connection is formed, and
    //successfully established. 
    //Once this connection is established a new filedescriptor will be returned,
    //all communication on this connection should be done using the new file
    //descriptor. 
    //-> The Second argument is a reference pointer to the address of the client on
    //the other side of the connection.
    //-> The Third argument is size of this structure.
    int newSd = accept( serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize );
    char buf1[1024];

    cout << "awaiting a connection... " << endl;
    if (newSd < 0){
        cout << "error connecting..." << endl;
    }

    while(serverSd > 0){
        cout << "got something coming in" << endl;
        strcpy(buf1, "Server Connected");
        write(newSd, buf1, 1024);
        //should block and wait
        read(newSd, buf1, 1024);

        
        signal( SIGIO, work);
        fcntl(newSd, F_SETOWN, getpid());
        fcntl(newSd, F_SETFL, FASYNC);











        /*
        cout << "Servers connected with client # ";
        cout << "serverSd " << serverSd << endl;

        cout << "Client: ";
        do{
            recv(newSd, buf1, 1024, 0);
            cout <<"Client speaking "  << buf1 << endl;

           double duration = ( clock() - start)/(double) CLOCKS_PER_SEC;
            cout << "printf: " << duration << endl;
        }while(*buf1 != '\0');
        do{
            cout << "Server " ;
            do{
                cin >> buf1;
                send(newSd, buf1, 1024, 0);
            }while(*buf1 != '\0');
            cout << "Client: ";
            do{
                recv(newSd, buf1, 1024, 0);
                cout << "Client speaking again1: " << buf1 << endl;

            }while(*buf1 != '\0');
        }while(*buf1 != '\0');
        */
    }
    
}

