///////////////////////////////////////////////////////////////////
// Danny Ly
// Program: client.cpp
// Description: make a socket between client and server
// sending data back and fourth and comparing multi-write
// Single write, and writeV, comparing their performance.
//
// Assumptions: 6 arguments to pass are of integer type,
// and follow the input sequence, error chcking will be done later.
//
//
////////////////////////////////////////////////////////////////////
#include <iostream>

#include <sys/types.h>  //socket, bind
#include <sys/socket.h> //socket, bind, listen, inet_ntoa
#include <netinet/in.h> //htonl, htons, inet_ntoa
#include <arpa/inet.h>  //inet_ntoa
#include <netdb.h>      //gethostbyName
#include <unistd.h>     //read, write, close
#include <string.h>     //bzero
#include <netinet/tcp.h>//SO_RUSEADDR
#include <sys/uio.h>    //writev

#include <cstdlib>      //convert from char to int lib
#include <sys/time.h>
//time libraries
#include <cstdio>
#include <ctime>

using namespace std;
const int BUFFSIZE = 1500;
int main(int argc, char *argv[]){
    //argc is the argument count that was passed
    //argv[0] is always the name of the program itself if there is 
    //if not an empty string.

    //After the 0th element and before argc list all arguments
    //argv[argc] is a NULL pointer.
    //cout << "there are " << argc << " arguemnts" << argv[0] << argv[1] <<
    //argv[2] << endl;
    std::cout<<"[+]Starting.... "<<std::endl;

    //list of expected arguments and their respective index's
    //argv[1] port
    //argv[2] repetition: repetition of sending a set of data buffers
    //argv[3] nbufs: number of data buffers
    //argv[4] bufsize: size of each data buffer(in bytes)
    //argv[5] serverIp: server IP name
    //argv[6] type: type of transfer scenario to test(1,2,or 3)

    //TODO: Error checking
    if (argc < 7) {
        cout << "error you are missing parameters" << endl;
        return 0;
    }
// Commented out for Local testing
    int port_num = atoi(argv[1]);   //port (last5 digits of STUID)
    int repetition = atoi(argv[2]); //repetition
    int num_buffs = atoi(argv[3]);   //nbufs
    int buff_size = atoi(argv[4]);  //bufsize
    const char *server_name = argv[5];    //server name, pointer to the IP number
    int type = atoi(argv[6]);       //type of process
    

/*
    int port_num = 22588;
    int repetition = 3;
    int num_buffs = 3;
    int buff_size = 1024;
    char *server_name = "127.0.0.1";
    int type = 2;
    
*/
    //retrieve a hostent structure
    //used to communicate to the server
    struct hostent *host = gethostbyname(server_name);
    if(host == NULL){

    cerr << "ERROR: count not find hostname: "<< &host << endl;
    return -1;
    }
    //connecting the socket
    //build the socket for the client side
    sockaddr_in sendSockAddr;
    bzero( (char*)&sendSockAddr, sizeof( sendSockAddr) ); //zero-initalize
    sendSockAddr.sin_family = AF_INET;              //Address Family internet
    sendSockAddr.sin_addr.s_addr = inet_addr( inet_ntoa(*(struct in_addr*)(*host->h_addr_list ) ));        
    sendSockAddr.sin_port = htons( port_num);       //Set port

    cout << "finished creating socket" << endl;
    //open a stream oriented socket with Intenet address family
    //socket discriptor
    //SOCKET() RETURN value: returns a NEW file descriptor for the new socket,
    //if failure or error -1 is returned, as well as errno is set.
    int clientSd = socket( AF_INET, SOCK_STREAM, 0);
    //checkign
    if(clientSd  < 0){
        cerr << "ERROR problems opening the sockt: " << clientSd << endl;
        close(clientSd);
        return -1;
        //attempt to close the socket
        
    }


    //if all success create the connection
    int resultCode = connect(clientSd, (sockaddr*)&sendSockAddr,sizeof(sendSockAddr));
    //CONNECT() RETURN value: -1 for failure errno is set. otherwise 0 returns
    if( resultCode < 0 ){
        cerr<< "ERROR connecting to server: " << resultCode << endl;
        close(clientSd);
        return -1;
    }


    //allocate the databuffer
    char databuffer[num_buffs][buff_size];

    struct timeval start;
    struct timeval lap;
    struct timeval stop;

    long lap_time;
    long total_time;

    //start the timer
    gettimeofday(&start, NULL);

    cout << "Sending the data " << endl;

    for(int i = 0; i < repetition; i++){
        //cout << "i = " << i << " repetiton amount: " << repetition << endl;
        if(type == 1){
            //multiwrite
            for(int j = 0; j < num_buffs; j++){
                //cout << "type 1: multiwrite j = " << j << endl;
                write(clientSd, databuffer[j], buff_size);
            }
        }else if(type == 2){
            
            struct iovec vector[num_buffs];
            for(int j = 0; j < num_buffs; j++){
               // cout << "Type 2 writev j = " << j << endl;
                vector[j].iov_base = databuffer[j];
                vector[j].iov_len = buff_size;
            }
            writev(clientSd, vector, num_buffs);

        }else{
            //cout << "Type 3 single write " << endl;
            write(clientSd, databuffer, num_buffs * buff_size);
        }
    }
    cout << "finished sending data " << endl;
    //record the duration of the work done
    gettimeofday(&lap, NULL); //lap the work finished time 
    //get the number of count the server ran/response
    int count;
    cout << "Waiting for response from server " << endl;
    read(clientSd, &count, sizeof(count));
   // cout << "count is: " << count << endl;

    //record completion time
    gettimeofday(&stop, NULL);

    //time recording and reporting
    lap_time = (lap.tv_sec - start.tv_sec) * 1000000 + (lap.tv_usec -
    start.tv_usec);

    total_time = (stop.tv_sec - start.tv_sec ) * 1000000 + (stop.tv_usec -
    start.tv_usec);

    cout << "Type of write: " << type << endl;
    cout << "data-sending time = " << lap_time << " usec" << endl;
    cout << "server response(complete cycle) time: " <<" usec " << total_time << endl;
    cout << "Number of reads: " << count << endl;
    //end the session
    close(clientSd);
    return 0;
}
