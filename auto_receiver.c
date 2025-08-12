#include <stdio.h>
#include "udp_codec.h"


void automatic_receiver(const char *playback, snd_pcm_t *pcm_handle_p,
                        snd_pcm_hw_params_t *params_p, int frame_size,
                        int channels, int sample_size, int sample_rate, int port) {
    AudioPacket packet;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); return; }

    struct sockaddr_in recv_addr = {0}, sender_addr = {0};
    socklen_t addr_len = sizeof(sender_addr);
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port   = htons(port);
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&recv_addr, sizeof(recv_addr)) < 0) {
        perror("bind"); close(sockfd); return;
    }

    // 1) SADECE İLK PAKETİ AL → codec tipini öğren
    ssize_t r = recvfrom(sockfd, &packet, sizeof(packet), 0,
                         (struct sockaddr*)&sender_addr, &addr_len);
    if (r <= 0) { perror("recvfrom"); close(sockfd); return; }

    uint8_t type = packet.codec_type;

    // Portu boşalt; asıl fonksiyon kendi socket’ini açsın
    close(sockfd);

    // 2) Kararı ver ve **yalnızca birini** çağır
    if (type == 1) {
        codec_receiver(playback, pcm_handle_p, params_p,
                       frame_size, channels, sample_size, sample_rate, port);
    } else {
        udp_receiver(playback, pcm_handle_p, params_p,
                     frame_size, channels, sample_size, sample_rate, port);
    }
}
