/////////////////////////
// Danny Ly
// Filename: Server.cpp
// Description: Server side of the respective client.cpp file, this program
// sends acknowledgments to the clients respective connections, and waits for
// incoming data, upon encountering data, will continue to call read, until the
// expected databuff is met, once finished sends back data pertaining to the
// time performance of read()
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
#include <netdb.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
using namespace std;
//////
// Description: called on I/O interrupt
//
//////

const int BUFFSIZE = 1500;      //as specified
const int NUM_CONNECTIONS = 5;  //as specified, max common connections
////////////////////////////////
//Descption: Function called during async socket connections to do the data
//processing and work
//
//
/////////////////////////////////


int num_reps;    //Server Repeitions
int serverSockDes; //server socket descriptor
int new_server_desc;    //returned connection file descritor

void work(int signal_identifier){
    cout << "Work function called " << endl;
    //Signal_identifier/SIGIO

    //allocate dataBufferer
    char dataBuffer[BUFFSIZE];
    
    //start the timer
    struct timeval start;
    struct timeval stop;
    long total_time;

    //start the timer
    gettimeofday(&start, NULL);
   
    //repeat calling read
    int count = 0;
   
    for(int i = 0; i < num_reps; i++){
        
        //for each repetition given, coninutally read the buffer until the number 

        //Read(): RETURN value, upon success , represents the number of bytes read
        //from the buffer, (zero indicating end of file), and the file position is
        //advanced by this number. IT IS NOT AN ERROR if the number is SMALLER than
        //the number of bytes requested; Ex: because fewer bytes are actually
        //avaliable right now( slow internet speeds, or some other reason for data
        //not all arriving, or read was interrupted by a signal). 

        //ON ERROR , -1 is returned, and errno is set appropriately

        int nRead = 0;          //number of reads accordingly
        while(nRead < BUFFSIZE){//number of reads also will repersent the bytes
        //cout << "nRead: " << nRead << endl;
        //cout << "Databuff" << dataBuffer << endl;
        //read from the buffer returned by READ(), we will increment the read by
        //the left over bytes that need to be still read, BUFFSIZE - nRead makes
        //sure we are reading all the data.
            int bytes_read = read(new_server_desc, dataBuffer, BUFFSIZE - nRead);
            nRead += bytes_read;
            count++;
            //cout << "bytes already read: " << bytes_read << endl;
        }
   }
    cout << "finished Reading data " << endl;
    //record the processing time
    
    gettimeofday(&stop, NULL);
    //double duration = ( clock() - start)/(double) CLOCKS_PER_SEC;
    total_time = (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec -
    start.tv_usec);
    //cout << "data-receiving time = " << duration << " usec" << endl;
    cout << "data-receiving time = " << total_time << " usec" << endl;
    //Send the number of reads back to the client, which also serves as
    //the acknowledgment.
    //dereference int count for the actual value
    write(new_server_desc, &count, sizeof(count) );
   
    //terminate the server process
    close(new_server_desc);
    close(serverSockDes);

    exit(0);
}
int main(int argc, char *argv[]){

    //Input argmuents that are expected
    //argv[1] Server IP port
    //argv[2] repetition: number of sending set of data buffers
    
    //TODO: Error checking
    
    int port_num = atoi(argv[1]);
    num_reps = atoi(argv[2]);
        
    //build the struct for establising a connection
    sockaddr_in acceptSockAddr;
    //zero initalize the struct
    bzero( (char*)&acceptSockAddr, sizeof( acceptSockAddr ) );
    //Address Family Internet
    //attribtes for the socket structure.
    acceptSockAddr.sin_family = AF_INET; 
    acceptSockAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    acceptSockAddr.sin_port = htons( port_num);
    
    //server Descriptor
    serverSockDes = socket( AF_INET, SOCK_STREAM, 0);

    //option to prompt OS to release the server port as soon as server process
    // is terminated
    const int on = 1; //flag variable to show port is in use

    setsockopt(serverSockDes, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof( int ));

    //bind socket to local address, giving socket descriptr, socked address,
    //data size

    //BIND() : RETURN val; on success 0, on failure -1 is returned, ERRNO is set
    //appropriatly
    int returnCode = bind( serverSockDes, ( sockaddr*)&acceptSockAddr, sizeof( acceptSockAddr ));
    
    //check if failure
    if(returnCode < 0){
        cerr << "Error: binding socket failed: " << returnCode << endl;
        close(serverSockDes);
        return returnCode;
    }

    //inform OS to listen up to 5 connection requests from client at a time
    listen(serverSockDes, NUM_CONNECTIONS);
    cout << "[SERVER] Begining listening ... " << endl;
    //create a new socket for arriving connections
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
    new_server_desc = accept( serverSockDes, (sockaddr *)&newSockAddr, &newSockAddrSize );

    //make the asynchronous split, using SIGIO flags

    signal(SIGIO, work);
    fcntl(new_server_desc, F_SETOWN, getpid());
    fcntl(new_server_desc, F_SETFL, FASYNC);

    //allow the server to go to sleep

    while(true){

        sleep(1000);
    }

    return 0;

    
}

