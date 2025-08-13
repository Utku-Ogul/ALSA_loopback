#include "udp.h"

void udp_sender(const char *capture, snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c,int frame_size,int channels, int sample_size, int sample_rate,int port ,const char *ip){
    

    AudioPacket packet;
    packet.codec_type = 0;
    
    //capture
    if (open_capture_device(capture, &pcm_handle_c, &params_c, channels, sample_rate)!=0){
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



    const int bytes_per_frame= channels* sample_size;
    const size_t header_len = offsetof(AudioPacket, payload);
    while (1){
        snd_pcm_readi(pcm_handle_c, packet.payload, frame_size);

        uint16_t bytes =(uint16_t)(frame_size*bytes_per_frame);

        packet.data_length = htons(bytes);


        ssize_t sent = sendto(sockfd, &packet, header_len + bytes, 0, (struct sockaddr *)&target_addr ,sizeof(target_addr));

        if (sent<0){
            perror("sendto");
        }
    }


}

int udp_receiver(const char *playback, snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size, int sample_rate, int port){

    AudioPacket packet;
    if (open_playback_device(playback, &pcm_handle_p, &params_p, channels, sample_rate)!=0){
        fprintf(stderr,"udp-open_playback_device!!!");
    };
    
    
    //UDP_receiver
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("socket");
        exit(1);
    }
    
    struct sockaddr_in recv_addr, sender_addr;
    socklen_t addr_len = sizeof(sender_addr);


    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family=AF_INET;
    recv_addr.sin_port=htons(port);
    recv_addr.sin_addr.s_addr=htonl(INADDR_ANY) ; // local harici için INADDR_ANY local inet_addr("127.0.0.1")

    bind(sockfd, (struct sockaddr*)&recv_addr, sizeof(recv_addr) );

    const size_t header_len =offsetof(AudioPacket, payload);
    const int bytes_per_frame= channels*sample_size;

    while (1)
    {
        ssize_t recv_len = recvfrom(sockfd, &packet,sizeof(packet), 0, (struct sockaddr*)&sender_addr,&addr_len);
        
        if(packet.codec_type==1){
            close(sockfd);
            return 1;
        }

        if(packet.codec_type!=0){
            fprintf(stderr, "incorrect type");
        };
        
        if(recv_len <0){
            perror("recvfrom");
            continue;
        }else if(recv_len>0){

            uint16_t payload_len = ntohs(packet.data_length);

            int frames = payload_len/ (sample_size*channels);
            
            int err=snd_pcm_writei(pcm_handle_p,packet.payload,frames);
            
            if(err<0){
                snd_pcm_recover(pcm_handle_p, err, 0);
            }
        }
    }

    return 0;

}; 