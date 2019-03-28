#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include <cstring>
#include <string>
#include <string.h>
#include <fstream>
#include <map>
#include <stdio.h>
#include <dirent.h>
#include <sstream>
// #include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>


#define BACKLOG 10

using namespace std;

int main(int argc, char const *argv[])
{

    //Phase IV

    //Command line parsing

        if (argc != 4)
        {
            cerr<<"Usage ./SimpleEmailServerPhase3   <portNum> <passwdfile> <user-database>\n";
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
    string userDBdir =argv[3];

    DIR *dir; //the directory
    struct dirent *dp;

    //open the directory
    if((dir  = opendir(userDBdir.c_str())) == NULL)
    {
        cerr<<"Invalid directory\n";
        exit(4);
    }
    
    //Declare
    int listener, newfd;
    int fdmax;
    fd_set master, readfds, writefds;
    map<int, string> sock_user;

    
    int yes = 1;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    unsigned int sin_size;
    fstream f;

    //Initialize
    FD_ZERO(&master);
    FD_ZERO(&readfds);
    
    // Getting socket file descriptor
    listener = socket(PF_INET, SOCK_STREAM, 0);
    
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes,
sizeof(int)) == -1) {
perror("setsockopt");
exit(1);
}

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), '\0', 8);

    //Set-up (Needs error correction)

    int bind_error = 0;

    // Binding to the specified port
    bind_error = bind(listener, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

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

   // Debugging
    // auto it = users.begin();
    //  for(; it != users.end(); it++)
    // {
    //     cout<<it->first<<" "<<it->second<<endl;
    // }
    


    cout<<"BindDone:"<<portNum<<"\n";
    int listen_error;

    // Listening to the socket with upto 10 users
    listen_error = listen(listener, BACKLOG);
    
    if (listen_error == -1)
    {
        cerr<<"Listen failed.\n";
        exit(4);
    }

    cout<<"ListenDone:"<<portNum<<"\n";
    //Add listener socket to the master set.
    FD_SET(listener, &master);
    fdmax = listener;
    sin_size = sizeof(struct sockaddr_in);
  
    while(true)
    {
    // List of sockets handled by the server;
    readfds = master;
    if (select(fdmax+1, &readfds, NULL, NULL, NULL) == -1) {
    cerr<<"select\n";
    exit(7);
    }

    for(size_t i = 0; i <= fdmax; i++)
    {
        if (FD_ISSET(i, &readfds)) {
            if (i==listener)
             {
                // New listen on the server, hence acceptance run.
                 
                newfd = accept(listener, (struct sockaddr *)&client_addr, &sin_size);
                if (newfd == -1)
                {
                    cerr<<"accept error\n";
                }
                // while(newfd == -1)
                // {
                    
                //     newfd = accept(listener, (struct sockaddr *)&client_addr, &sin_size);
     
                // }

                char Log_Info[1024];
                int bytes_recvd = recv(newfd, Log_Info, 1024, 0);
                char clientIPaddr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(client_addr.sin_addr), clientIPaddr, INET_ADDRSTRLEN);
                cout<<"Client:"<<clientIPaddr<<":"<<client_addr.sin_port<<"\n";
                string username, passwd;

                // *Parse the string User: username Pass: passwd*
                istringstream logss(Log_Info);
                int isvalid = 1;
                char* p;
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
                    continue;
                
                }
                
                if (word[0] == "User:" && word[2] == "Pass:" && isvalid) {
                    username = word[1];
                    passwd = word[3];
                }
                else
                {
                    cout<<"Unknown Command\n";
                    close(newfd);
                    continue;
                    // exit(0);
                }
                
                
                

                if (users.find(username) == users.end()) 
                {
                    cout<<"Invalid User\n";
                    close(newfd);
                    continue;
                    // exit(0);
                }
                else
                {
                    if (users[username] != passwd)
                    {
                        cout<<"Wrong Passwd\n";
                        close(newfd);
                        continue;
                    }
                    else
                    {
                        string welcome_msg = "Welcome " + username + "\n";
                        cout<<welcome_msg;
                        const char* wel_msg = (welcome_msg + '\0').c_str();
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


                        FD_SET(newfd, &master);
                        if (newfd > fdmax) 
                        {
                        // keep track of the maximum
                            fdmax = newfd;
                        }
                        if (sock_user.find(newfd) != sock_user.end()) {
                            sock_user[newfd] = username;
                        }
                        else
                        {
                            sock_user.insert(make_pair(newfd, username));
                        }
                    }    
                }
    

             
            }
            else
            {
             
                char next_msg[1024];

                int bytes_recvd = recv(i, next_msg, 1024, 0);
                if (bytes_recvd <= 0) {
                    close(i);
                    FD_CLR(i, &master);
                    continue;
                }
                else
                {
                    string s(next_msg); // Command

                    //Command: quit
                    if (strcmp(next_msg, "quit")== 0) {
                            string goodbye_msg = "Bye "  + sock_user[i] + "\n";
                            cout<<goodbye_msg;
                            close(i);
                            FD_CLR(i, &master);
                            continue;   
                    }
                    //Command: LIST
                    else if (strcmp(next_msg, "LIST")== 0)
                    {
                        string userdir = userDBdir + sock_user[i];
                        // cout<<userdir<<endl;
                        DIR *diru; //the directory
                        struct dirent *dpu;

                        //open the directory
                        if((diru  = opendir(userdir.c_str())) == NULL)
                        {
                            cout<<sock_user[i]<<": Folder Read Fail\n";
                            close(i);
                            FD_CLR(i, &master);
                            continue;
                        }
                        else
                        {
                        int i =0;
                        while (dpu = readdir (diru))
                            {
                                i++;
                            }
                            i = i -2;
                        (void) closedir (diru);
                        string list_msg = sock_user[i] + ": No of messages "+to_string(i)+'\n';
                        cout<<list_msg;
                        const char* l_msg = list_msg.c_str();
                            int bytes_sent = send(i, l_msg,strlen(l_msg)+1, 0); 
                            if (bytes_sent == -1)
                            {
                                cerr<<"Not sent.\n";
                                exit(2);
                            }
                            else if (bytes_sent != list_msg.length()+1) 
                            {
                                cerr<<"Data not sent completely.\n";
                                exit(2);
                            }
                    
                        }
                    
                    }
                    // Command RETRV <ID>
                    else if (s.find("RETRV ")==0) {
                        int id_pos = s.find(" ");
                        int id = stoi(s.substr(id_pos +1));
                        // cout<<id<<"\n";
                        string userdir = userDBdir + sock_user[i];
                        // cout<<userdir<<endl;
                        DIR *diru; //the directory
                        struct dirent *dpu;

                        //open the directory
                        if((diru  = opendir(userdir.c_str())) == NULL)
                        {
                            cout<<"Message Read Fail\n";
                            close(i);
                            FD_CLR(i, &master);
                            continue;
                        }
                        else
                        {
                        bool found = false;
                        string filename;
                        while (dpu = readdir (diru))
                            {
                                filename = string(dpu->d_name);
                                if (filename == "." || filename == "..")
                                {
                                    continue;
                                }
                                string temp = filename.substr(0,filename.find("."));
                                istringstream fss(temp);
                                int file_id;
                                fss>>file_id;
                                
                                if (file_id == id) {
                                    found = true;
                                    break;
                                }
                                
                            }
                        (void) closedir (diru);
                        if (!found)
                        {
                            cout<<"Message Read Fail\n";
                            close(i);
                            FD_CLR(i, &master);
                            continue;
                        }
                    fstream file;
                        string opfilename  = userdir + "/" + filename; 
                    file.open(opfilename, ios::in|ios::binary);
                        // cout<<"Name of the file opened: "<<opfilename<<endl;
                        if (! file.good()) 
                        {
                            cerr<<"Message Read Fail\n";
                              close(i);
                            FD_CLR(i, &master);
                            continue;
                        }
                        else{
                        const char* file_name_msg = (filename + '\0').c_str();
                        int bytes_sent = send(i, file_name_msg,1024, 0); 

                        file.seekg(0, ios::end);
                        streampos size = file.tellg();
                        // cout<<"File size: "<<size<<"\n";
                        const char* file_size = (to_string(size) + '\0').c_str();
                        bytes_sent = send(i, file_size,32, 0); 

                        int ctr = 0;
                        file.seekg (0, ios::beg);
                        int remainder=1024;
                        int fs =size; 
                        cout<<sock_user[i]<<": Transferring Message "<<id<<" \n";
                        for(int j = 0; j < fs/1024; j++)
                        {
                            // cout<<"In for loop\n";
                            char buffer[1024];
                            file.read(buffer, 1024);
                            bytes_sent = send(i, buffer, 1024, 0);
                            // cout<<"pack"<<ctr<<" "<<"Bytes: "<<bytes_sent<<endl;
                            ctr++;
                            remainder = remainder - bytes_sent;
                            while(remainder > 0){
                                /* code */
                            cout<<bytes_sent<<endl;
                            cout<<"Sending incomplete\n";
                            cout<<bytes_sent<<endl;
                            bytes_sent = send(i, buffer, remainder, 0);
                            remainder = remainder - bytes_sent;
                                
                            }
                            

                        }
                        if (fs % 1024> 0)   
                        {
                            // cout<<"sending remaining bites\n";
                            char buffer[fs%1024];
                            
                            file.read(buffer, fs%1024);
                            bytes_sent = send(i, buffer, fs%1024, 0);
                            // cout<<"pack"<<ctr<<" "<<"Bytes: "<<bytes_sent<<endl;
                        }
                        file.close();
                        ctr = 0;
                        }
                    }
                    }
                    else
                    {
                        cout<<"Unknown command\n";
                         close(i);
                        FD_CLR(i, &master);
                        continue;
                    }
                }
                
                

            }
            
            
        }
        
    }

    }

    f.close();
    return 0;
}
