#include "udp.h"

int create_socket(){

    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0){
        perror("socket");
        exit(1);
    };

    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(5000); //5000 portu için
    target_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    return sockfd;
};

struct sockaddr_in target_addr(const char *ip, int port){
    
}