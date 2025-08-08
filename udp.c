#include "udp.h"

void udp_sender(const char *capture, snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c,int frame_size,int channels, int sample_size, int sample_rate,int port ,char ip){
    
    int buffer_size=frame_size * channels * sample_size;
    int16_t *buffer=malloc(buffer_size);

    //capture
    if (open_capture_device(capture, pcm_handle_c, params_c, channels, sample_rate)!=0){
        fprintf(stderr,"udp-open_capture_device!!!");
        return;
    };

    //UDP_sender

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("socket");
        exit(1);
    }
    
    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(port); //5000 portu için
    target_addr.sin_addr.s_addr = inet_addr(ip);




    while (1){
        snd_pcm_readi(pcm_handle_c, buffer, frame_size);
        ssize_t sent = sendto(sockfd, buffer, frame_size*channels*sample_size, 0, (struct sockaddr *)&target_addr ,sizeof(target_addr));

        if (sent<0){
            perror("sendto");
        }
    }


}

void udp_receiver(const char *playback, snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size, int sample_rate, int port){

    
    //playback
    int buffer_size=frame_size * channels * sample_size;
    int16_t *buffer=malloc(buffer_size);
        
    if (open_playback_device(playback, pcm_handle_p, params_p, channels, sample_rate)!=0){
        fprintf(stderr,"udp-open_playback_device!!!");
        return;
    };

    
    //UDP_receiver
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("socket");
        exit(1);
    }
    
    struct sockaddr_in recv_addr, sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    recv_addr.sin_family=AF_INET;
    recv_addr.sin_port=htons(port);
    recv_addr.sin_addr.s_addr=INADDR_ANY ; // local harici için INADDR_ANY local inet_addr("127.0.0.1")

    bind(sockfd, (struct sockaddr*)&recv_addr, sizeof(recv_addr) );


    while (1)
    {
        ssize_t recv_len = recvfrom(sockfd, buffer,(frame_size * channels * sample_size), 0, (struct sockaddr*)&sender_addr,&addr_len);
        
        if(recv_len <0){
            perror("recvfrom");
            continue;
        }else if(recv_len>0){
            int frames = recv_len/ (sample_size*channels);
            
            int err=snd_pcm_writei(pcm_handle_p,buffer,frames);
            
            if(err<0){
                snd_pcm_recover(pcm_handle_p, err, 0);
            }
        }
    }
    
    
};