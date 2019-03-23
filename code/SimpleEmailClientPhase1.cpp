#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include <cstring>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

int main(int argc, char const *argv[])
{
    
    //Phase I

    //Command line parsing

    if (argc != 4)
    {
        cerr<<"Usage ./SimpleEmailClientPhase1 <serverIPAddr:port> <user-name> <passwd> \n";
        exit(1); 
    }
    
    string s = argv[1];
    int pos = s.find(':');

    if (pos == string::npos)
    {
        cerr<<"Wrong Input format, Connection failed.\n";
        exit(2);
    }

    string username, passwd, serverIPaddr;
    
    username = argv[2];
    passwd = argv[3];
    serverIPaddr = s.substr(0, pos);

    int SERVER_PORT;
    istringstream ss(s.substr(pos+1, s.size()));

    if (!(ss >> SERVER_PORT)) //Convert the string into a stream and 
                              // then read the SERVERPORT from the stream 
    {
        cerr <<"Connection failed, invalid Port Number"<<'\n';
        exit(2);
    } 
    else if (!ss.eof())
    {
        cerr <<"Connection failed, invalid Port Number"<<'\n';
        exit(2);
    }

    // Debug
    // cout<<"Username: "<<username<<" Password: "<<passwd<<" ServerIP: "<<serverIPaddr<<" Server_port: "<<SERVER_PORT<<'\n';
    
    int sockfd;
    struct sockaddr_in dest_addr; // will hold the destination addr
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    dest_addr.sin_family = AF_INET;        // host byte order
    dest_addr.sin_port = htons(SERVER_PORT); // network byte order
    // dest_addr.sin_addr.s_addr = inet_addr(serverIPaddr.c_str());
    int addr_check = inet_pton(AF_INET,serverIPaddr.c_str(), &(dest_addr.sin_addr));
    
    if (addr_check <= 0) {
        cerr<<"Invalid address\n";
        exit(2);
    }
    

    memset(&(dest_addr.sin_zero), '\0', 8); // zero the rest of the struct
    
    int conn_success = connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
    if (conn_success == -1)
    {
        cerr<<"Connection failed.\n";
        exit(2);
    }
    else
    {
        cout<<"ConnectDone:"<<serverIPaddr<<":"<<SERVER_PORT<<'\n';
    }
    
    string login_msg = "User: " + username + " Pass: "+ passwd;
    const char* msg  = login_msg.c_str();
    int bytes_sent = send(sockfd, msg, login_msg.length(), 0); 


    if (bytes_sent == -1)
    {
        cerr<<"Not sent.\n";
        exit(2);
    }
    else if (bytes_sent != login_msg.length()) {
        cerr<<"Data not sent completely.\n";
        exit(2);
    }
    char welcome_msg[2048];
    int bytes_recvd = recv(sockfd, welcome_msg, 2048, 0);
    if (bytes_recvd == -1)
    {
        cout<<"Client didn't recv\n";
        exit(2);
    }
    cout<<welcome_msg;
    

    const char* logout_msg  = "quit";
    bytes_sent = send(sockfd, logout_msg,strlen(logout_msg), 0); 

    if (bytes_sent == -1)
    {
        cerr<<"Not sent.\n";
        exit(2);
    }
    else if (bytes_sent != strlen(logout_msg)) {
        cerr<<"Data not sent completely.\n";
        exit(2);
    }
    
    // char goodbye_msg[2048];
    // bytes_recvd = recv(sockfd, goodbye_msg, 2048, 0);
    
    // if (bytes_recvd == -1)
    // {
    //     cout<<"Client didn't recv\n";
    //     exit(2);
    // }
    // cout<<goodbye_msg;
    

    return 0;
}

