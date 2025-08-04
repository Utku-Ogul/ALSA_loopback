#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int main(){

    int sockfd = socet(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0){
        perror("socket");
        exit(1);

    }

    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(12345);
    target_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    

    return 0;
}