#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in recv_addr, sender_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(sender_addr);

    // 1. UDP socket oluştur
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // 2. Alıcı soket adresi tanımla ve porta bağla
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = INADDR_ANY;  // Tüm arayüzlerden veri al
    recv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&recv_addr, sizeof(recv_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    printf("UDP alıcı başlatıldı. Port: %d\n", PORT);

    // 3. Sonsuz döngüde paketleri al
    while (1) {
        ssize_t recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                    (struct sockaddr*)&sender_addr, &addr_len);

        if (recv_len < 0) {
            perror("recvfrom");
            continue;
        }

        buffer[recv_len] = '\0';  // null-terminator
        printf("Alınan UDP mesajı (%s:%d): %s\n",
               inet_ntoa(sender_addr.sin_addr),
               ntohs(sender_addr.sin_port),
               buffer);
    }

    close(sockfd);
    return 0;
}
