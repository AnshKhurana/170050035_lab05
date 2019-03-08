#include <iostream>
#include <sys/socket.h>
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

        int sockfd;
        struct sockaddr_in dest_addr; // will hold the destination addr
        sockfd = socket(PF_INET, SOCK_STREAM, 0);
        dest_addr.sin_family = AF_INET;        // host byte order
        dest_addr.sin_port = htons(SERVER_PORT); // network byte order
        dest_addr.sin_addr.s_addr = inet_addr(serverIPaddr.c_str());
        memset(&(dest_addr.sin_zero), '\0', 8); // zero the rest of the struct
        
        connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));

    return 0;
}

