#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#define SERVERPORT 8000
#define BACKLOG 10

using namespace std;

int main(int argc, char const *argv[])
{
    //Declare
    int sockfd, newfd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int sin_size;

    //Initialize
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    my_addr.sin_family = PF_INET;
    my_addr.sin_port = htons(SERVERPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), '\0', 8);

    cout<<"Initialization done"<<endl;

    //Set-up (Needs error correction)

    bind(sockfd, (struct sock_addr *)&my_addr, sizeof(struct sockaddr));

    listen(sockfd, BACKLOG);
    sin_size = sizeof(struct sockaddr_in);
    newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    
    cout<<newfd<<endl;
    
    return 0;
}
