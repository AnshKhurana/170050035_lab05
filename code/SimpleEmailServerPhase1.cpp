#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include <cstring>
#include <string>
#include <fstream>
#include <map>
#include <sstream>

#define BACKLOG 10

using namespace std;

int main(int argc, char const *argv[])
{

    //Phase I

    //Command line parsing

        if (argc != 3)
        {
            cerr<<"Usage ./SimpleEmailServerPhase1 <portNum> <passwdfile>\n";
            exit(1); 
        }
    int portNum;
    istringstream ss(argv[1]);
    if (!(ss >> portNum)) 
    {
        cerr <<"Invalid Port Number"<<'\n';
        exit(2);
    } 
    else if (!ss.eof()) {
        cerr <<"Invalid Port Number"<<'\n';
        exit(2);
    }


    string passfilename = argv[2];

    //Declare
    int sockfd, newfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    unsigned int sin_size;
    fstream f;


    //Initialize
    
    f.open(passfilename, ios::in);
    if (! f.good()) {
        cerr<<"Password file not present or not readable\n";
        exit(3);
    }
    
    map<string, string> users; 
    
    string user_c, pass_c;
    f>>user_c;
    f>>pass_c;
    users.insert(make_pair(user_c, pass_c));
    while(!f.eof()){
        f>>user_c;
        f>>pass_c;
        users.insert(make_pair(user_c, pass_c));
    }

   
    
    // Debugging
    // auto it = users.begin();
    //  for(; it != users.end(); it++)
    // {
    //     cout<<it->first<<" "<<it->second<<endl;
    // }
    

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), '\0', 8);

    //Set-up (Needs error correction)

    int bind_error = 0;

    bind_error = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

    if (bind_error == -1)
    {
        cerr<<"Bind failed, invalid port.\n";
        exit(2);
    }

    cout<<"BindDone:"<<portNum<<"\n"<<endl;
    int listen_error;

    listen_error = listen(sockfd, BACKLOG);
    
    if (listen_error == -1)
    {
        cerr<<"Listen failed.\n";
        exit(4);
    }

    cout<<"ListenDone:"<<portNum<<"\n";

    sin_size = sizeof(struct sockaddr_in);
    newfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
    
    while(newfd == -1){
   
    newfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
     
    }

    char username[1024];
    int bytes_recvd = recv(newfd, username, 1024, 0);
    
    char passwd[1024];
    int bytes_recvd = recv(newfd, passwd, 1024, 0);
  
    char clientIPaddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), clientIPaddr, INET_ADDRSTRLEN);

    cout<<"Client:"<<clientIPaddr<<":"<<client_addr.sin_port<<"\n";

    if (users.find(username) == users.end()) {
        cout<<"Invalid User\n";
        close(newfd);

    }
    else
    {
        if (strcmp(users[username], passwd) != 0) {
            cout<<"Wrong Passwd";
        }
        else
        {
            cout<<"Welcome "<<username<<'\n';
        }
        
        
    }
    
    

    // while(recv(newfd, s, 1024, 0)){
        
    // }
        
    
    f.close();
    return 0;
}
