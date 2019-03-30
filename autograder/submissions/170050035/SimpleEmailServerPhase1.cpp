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
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>


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
    
    // Debugging
    // auto it = users.begin();
    //  for(; it != users.end(); it++)
    // {
    //     cout<<it->first<<" "<<it->second<<endl;
    // }
    
    // Getting socket file descriptor
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), '\0', 8);

    //Set-up (Needs error correction)

    int bind_error = 0;

    int yes=1;
//char yes=’1’; // Solaris people use this
// lose the pesky "Address already in use" error message
    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
    perror("Bind failed.\n");
    exit(2);
    }

    // Binding to the specified port
    bind_error = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

    if (bind_error == -1)
    {
        cerr<<"Bind failed, invalid port.\n";
        exit(2);
    }
        
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

   


    cout<<"BindDone:"<<portNum<<"\n";
    int listen_error;

    // Listening to the socket with upto 10 users
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

    char Log_Info[1024];
    int bytes_recvd = recv(newfd, Log_Info, 1024, 0);
    char clientIPaddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), clientIPaddr, INET_ADDRSTRLEN);
    cout<<"Client:"<<clientIPaddr<<":"<<client_addr.sin_port<<"\n";
    string username, passwd;

    // *Parse the string User: username Pass: passwd*
    istringstream logss(Log_Info);
    int isvalid = 1;
    string word[4];
    for(size_t i = 0; i < 4; i++)
    {
        if (logss>>word[i])
        {
            //Debug:  cout<<word[i]<<endl;
            continue;
        }
        else
        {
            isvalid = 0;
            break;
        }
        
    }
    if (!isvalid) {
        cout<<"Unknown Command\n";
        close(newfd);
        exit(0);
    }
    
    if (word[0] == "User:" && word[2] == "Pass:" && isvalid) {
        username = word[1];
        passwd = word[3];
    }
    else
    {
        cout<<"Unknown Command\n";
        close(newfd);
        exit(0);
    }
    
    
    

    if (users.find(username) == users.end()) 
    {
        cout<<"Invalid User\n";
        close(newfd);
        exit(0);
    }
    else
    {
        if (users[username] != passwd)
        {
            cout<<"Wrong Passwd\n";
            close(newfd);
            exit(0);
        }
        else
        {
            string welcome_msg = "Welcome " + username + "\n";
            cout<<welcome_msg;
            const char* wel_msg = welcome_msg.c_str();
            int bytes_sent = send(newfd, wel_msg,strlen(wel_msg)+1, 0); 
            if (bytes_sent == -1)
            {
                cerr<<"Not sent.\n";
                exit(2);
            }
            else if (bytes_sent != welcome_msg.length()+1) 
            {
                cerr<<"Data not sent completely.\n";
                exit(2);
            }
        }    
    }
    
    char next_msg[1024];

    bytes_recvd = recv(newfd, next_msg, 1024, 0);
    if (strcmp(next_msg, "quit")== 0) {
            string goodbye_msg = "Bye "  + username + "\n";
            cout<<goodbye_msg;
            close(newfd);
            exit(0);        
    }
    else
    {
        cout<<"Unknown command\n";
        close(newfd);
        exit(0);
    }
    
    f.close();
    close(newfd);
    return 0;
}
