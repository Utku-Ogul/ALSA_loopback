#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <alsa/asoundlib.h>

void udp_receiver(snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,
                  int frame_size, int channels, int sample_size) {
    
    char buffer[frame_size * channels * sample_size];

    // ALSA playback ayarları
    snd_pcm_hw_params_set_access(pcm_handle_p, params_p, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle_p, params_p, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle_p, params_p, channels);
    snd_pcm_hw_params_set_rate(pcm_handle_p, params_p, 48000, 0);
    snd_pcm_hw_params(pcm_handle_p, params_p);
    snd_pcm_prepare(pcm_handle_p);

    // UDP socket oluştur
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // Dinlenecek IP ve port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);              // 5000 portunu dinle
    server_addr.sin_addr.s_addr = INADDR_ANY;        // Her IP'den kabul et

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(1);
    }

    // Alıcı döngüsü
    while (1) {
        ssize_t received = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (received < 0) {
            perror("recvfrom");
            continue;
        }

        // PCM verisini hoparlöre gönder
        snd_pcm_writei(pcm_handle_p, buffer, frame_size);
    }

    // Temizlik
    snd_pcm_close(pcm_handle_p);
    close(sockfd);
}
