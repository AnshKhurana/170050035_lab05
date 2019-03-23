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

    if (!(ss >> SERVER_PORT)) 
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
        cerr<<"Connection failed.";
        exit(2);
    }

    const char* msg  = username.c_str();
    int bytes_sent = send(sockfd, msg, username.length(), 0); 

    if (bytes_sent == -1)
    {
        cerr<<"Not sent.\n";
        exit(2);
    }
    else if (bytes_sent != username.length()) {
        cerr<<"Data not sent completely.\n";
        exit(2);
    }
    

    const char* msgp  = passwd.c_str();
    bytes_sent = send(sockfd, msg, passwd.length(), 0); 

    if (bytes_sent == -1)
    {
        cerr<<"Not sent.\n";
        exit(2);
    }
    else if (bytes_sent != passwd.length()) {
        cerr<<"Data not sent completely.\n";
        exit(2);
    }

    string x;
    
    while(x!= "quit"){
        
    const char* msgx  = x.c_str();
    bytes_sent = send(sockfd, msg, x.length(), 0); 

    if (bytes_sent == -1)
    {
        cerr<<"Not sent.\n";
        exit(2);
    }
    else if (bytes_sent != x.length()) {
        cerr<<"Data not sent completely.\n";
        exit(2);
    }
    }
    

    return 0;
}

