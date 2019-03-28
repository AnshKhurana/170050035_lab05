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
#include <fstream>
#include <vector>
#include <sys/stat.h>

using namespace std;

int main(int argc, char const *argv[])
{
    
    //Phase III

    //Command line parsing

    if (argc != 6)
    {
        cerr<<"Usage ./SimpleEmailClientPhase3 <serverIPAddr:port> <user-name> <passwd> <list-of-messages> <local-folder> \n";
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

    string list_of_messages =  argv[4];
    vector<int> msglist;

   stringstream lss(list_of_messages);

    int i;
    int next = 0;
    while (lss >> i)
    {
        next = next*10 + i; 

        if (lss.peek() == ',')
            {
                msglist.push_back(next);
                next = 0;
                lss.ignore();
            }
    }

    msglist.push_back(next);
    size_t l = std::count(list_of_messages.begin(), list_of_messages.end(),',');
    if (msglist.size() != (l+1) || list_of_messages[list_of_messages.length()-1] == ',' ) {
        cerr<<"Not a list of numbers/ill-formatted.\n";
        exit(3);
    }
    
    string local_folder = argv[5];
    struct stat buffer;
    if (stat (local_folder.c_str(), &buffer) == 0)
    {
        string cmd = "rm -r " + local_folder;
        int del =system(cmd.c_str());
        if (del != 0) {
            cerr<<"Unable to create/access folder.\n";
            exit(4);
        }
        
    }
   
    int ncreated =  mkdir(local_folder.c_str(), 0777);
    if (ncreated) {
        cerr<<"Unable to create/access folder.\n";
        exit(4);
    }
    
    


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
    int bytes_sent = send(sockfd, msg, login_msg.length()+1, 0); 


    if (bytes_sent == -1)
    {
        cerr<<"Not sent.\n";
        exit(2);
    }
    else if (bytes_sent != login_msg.length()+1) {
        cerr<<"Data not sent completely.\n";
        exit(2);
    }
    char welcome_msg[1024];
    int bytes_recvd = recv(sockfd, welcome_msg, 1024, 0);
    if (bytes_recvd == -1 || bytes_recvd == 0)
    {
        exit(2);
    }
    else{
        cout<<welcome_msg;
    }
    // cout<<"Welcome message number of bytes received."<<bytes_recvd<<"\n";
    
    // const char* list_msg  = "LIST";
    // bytes_sent = send(sockfd, list_msg,strlen(list_msg)+1, 0); 

    // if (bytes_sent == -1)
    // {
    //     cerr<<"Not sent.\n";
    //     exit(2);
    // }
    // else if (bytes_sent != strlen(list_msg)+1) {
    //     cerr<<"Data not sent completely.\n";
    //     exit(2);
    // }
    
    // char list_msg_reply[1024];
    // bytes_recvd = recv(sockfd, list_msg_reply, 1024, 0);
    // if (bytes_recvd == -1 || bytes_recvd == 0)
    // {
       
    //     exit(2);
    // }
    // else
    // {
    // cout<<list_msg_reply;

    // }
    for(size_t i = 0; i < msglist.size(); i++)
    {
        string ret = "RETRV " + to_string(msglist[i]);
        const char* ret_msg  = ret.c_str();
        bytes_sent = send(sockfd, ret_msg,strlen(ret_msg)+1, 0); 
        char ret_msg_reply[1024];
        bytes_recvd = recv(sockfd, ret_msg_reply, 1024, 0);
        // cout<<"Bytes recvd while filename "<<bytes_recvd<<endl;
        if (bytes_recvd == -1 || bytes_recvd == 0)
        {
            exit(2);
        }
        else
        {
        char size_msg[32];
        bytes_recvd = recv(sockfd, size_msg, 32, 0);
        // cout<<"Bytes recvd while size_msg "<<bytes_recvd<<endl;
        
        // cout<<"Downloading file "<<ret_msg_reply<<" with size = "<<size_msg<<endl;

        fstream fw;
        string opfilename = local_folder + ret_msg_reply; 
        fw.open(opfilename, ios::out|ios::binary);
        int total_bytes = 0;
        int size = stoi(size_msg);
        // cout<<"File size: "<<size<<endl;
        int ctr;

        for(size_t i = 0; i < size/1024; i++)
        {
            // cout<<"Started receiving\n";
            char buffer[1024];
            bytes_recvd = recv(sockfd, buffer, 1024, 0);
            // cout<<"pack "<<ctr<<" "<<bytes_recvd<<endl;
            ctr++;
            total_bytes = total_bytes + bytes_recvd;
            fw.write(buffer, bytes_recvd);
            
        }
        if (size%1024 != 0) {
            // cout<<"Receiving remaining bytes\n";
            char buffer[size%1024];
            bytes_recvd = recv(sockfd, buffer, size%1024, 0);
            // cout<<"pack "<<ctr<<" "<<bytes_recvd<<endl;
            ctr++;
            total_bytes = total_bytes + bytes_recvd;
            fw.write(buffer, bytes_recvd);
        }
        
        
        // while(total_bytes < size){
        //     cout<<"Starting receiving\n";
        //     char buffer[1024];
        //     bytes_recvd = recv(sockfd, buffer, 71, 0);
        //     cout<<"pack"<<ctr<<" "<<bytes_recvd<<endl;
        //     ctr++;
        //     total_bytes = total_bytes + bytes_recvd;
        //     fw.write(buffer, bytes_recvd);
        //     // if(bytes_recvd < 1024)
        //     // {
        //     //     fw.close();
        //     //     break;
        //     // } 
        // }
        ctr = 0;
        fw.close();
        cout<<"Downloaded Message "<<msglist[i]<<"\n";

        }
    }
    const char* logout_msg  = "quit";
    bytes_sent = send(sockfd, logout_msg,strlen(logout_msg)+1, 0); 

    if (bytes_sent == -1)
    {
        cerr<<"Not sent.\n";
        exit(2);
    }
    else if (bytes_sent != strlen(logout_msg)+1) {
        cerr<<"Data not sent completely.\n";
        exit(2);
    }
    

    return 0;
}

