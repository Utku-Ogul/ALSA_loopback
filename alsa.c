#include "alsa.h"

const snd_pcm_format_t test_formats[] = {
    SND_PCM_FORMAT_S8,
    SND_PCM_FORMAT_U8,
    SND_PCM_FORMAT_S16_LE,
    SND_PCM_FORMAT_S32_LE,
    SND_PCM_FORMAT_FLOAT_LE,
    SND_PCM_FORMAT_FLOAT64_LE
};


//mikrefon
void print_capture_device_info(snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c) {
    printf("\n\n capture \n\n");
    printf("Açilan cihaz: %s\n\n", snd_pcm_name(pcm_handle_c));

    snd_pcm_info_t *info;
    snd_pcm_info_malloc(&info);
    snd_pcm_info(pcm_handle_c ,info);
    printf("Cihaz ismi: %s \n", snd_pcm_info_get_name(info));


    printf("\nDesteklenen formatlar:\n");
    for (int i = 0; i < 6; i++) {
        if (snd_pcm_hw_params_test_format(pcm_handle_c, params_c, test_formats[i]) == 0) {
            printf("- %s\n", snd_pcm_format_name(test_formats[i]));
        }
    }

    unsigned int min_rate, max_rate;
    int dir;
    snd_pcm_hw_params_get_rate_min(params_c, &min_rate, &dir);
    snd_pcm_hw_params_get_rate_max(params_c, &max_rate, &dir);
    printf("\nDesteklenen örnekleme hizi:\n- Min: %u Hz\n- Max: %u Hz\n", min_rate, max_rate);

    unsigned int min_channels, max_channels;
    snd_pcm_hw_params_get_channels_min(params_c, &min_channels);
    snd_pcm_hw_params_get_channels_max(params_c, &max_channels);
    printf("\nDesteklenen kanal sayisi:\n- Min: %u\n- Max: %u\n", min_channels, max_channels);

    snd_pcm_info_free(info);
}

//hoparlör
void print_playback_device_info(snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p) {
    printf("\n\n playback\n\n");
    printf("Açilan cihaz: %s\n\n", snd_pcm_name(pcm_handle_p));

    snd_pcm_info_t *info;
    snd_pcm_info_malloc(&info);
    snd_pcm_info(pcm_handle_p, info);
    printf("Cihaz ismi: %s \n", snd_pcm_info_get_name(info));

    printf("\nDesteklenen formatlar:\n");
    for (int i = 0; i < 6; i++) {
        if (snd_pcm_hw_params_test_format(pcm_handle_p, params_p, test_formats[i]) == 0) {
            printf("- %s\n", snd_pcm_format_name(test_formats[i]));
        }

    }
    unsigned int min_rate, max_rate;
    int dir;
    snd_pcm_hw_params_get_rate_min(params_p, &min_rate, &dir);
    snd_pcm_hw_params_get_rate_max(params_p, &max_rate, &dir);
    printf("\nDesteklenen örnekleme hizi:\n- Min: %u Hz\n- Max: %u Hz\n", min_rate, max_rate);

    unsigned int min_channels, max_channels;
    snd_pcm_hw_params_get_channels_min(params_p, &min_channels);
    snd_pcm_hw_params_get_channels_max(params_p, &max_channels);
    printf("\nDesteklenen kanal sayisi:\n- Min: %u\n- Max: %u\n", min_channels, max_channels);

    snd_pcm_info_free(info);
}

void loopback(snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c,snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size){

    int buffer_size=frame_size * channels * sample_size;
    int16_t *buffer=malloc(buffer_size);

    //capture
    snd_pcm_hw_params_set_access(pcm_handle_c,params_c,SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle_c,params_c,SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle_c, params_c, channels);
    snd_pcm_hw_params_set_rate(pcm_handle_c,params_c,48000,0);
    snd_pcm_hw_params(pcm_handle_c,params_c);
    snd_pcm_prepare(pcm_handle_c);
    
    //playback
    snd_pcm_hw_params_set_access(pcm_handle_p,params_p,SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle_p,params_p,SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle_p, params_p, channels);
    snd_pcm_hw_params_set_rate(pcm_handle_p,params_p,48000,0);
    snd_pcm_hw_params(pcm_handle_p,params_p);
    snd_pcm_prepare(pcm_handle_p);



    while (1) {
        snd_pcm_readi(pcm_handle_c, buffer, frame_size);
        snd_pcm_writei(pcm_handle_p, buffer, frame_size);
    }

    
    snd_pcm_close(pcm_handle_p);
    snd_pcm_close(pcm_handle_c);

    free(buffer);

}


void udp_sender(snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c,snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size){
    
    int buffer_size=frame_size * channels * sample_size;
    int16_t *buffer=malloc(buffer_size);

    //capture
    snd_pcm_hw_params_set_access(pcm_handle_c,params_c,SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle_c,params_c,SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle_c, params_c, channels);
    snd_pcm_hw_params_set_rate(pcm_handle_c,params_c,48000,0);
    snd_pcm_hw_params(pcm_handle_c,params_c);
    snd_pcm_prepare(pcm_handle_c);


    //UDP_sender
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0){
        perror("socket");
        exit(1);

    }

    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(5000); //5000 portu için
    target_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


    while (1){
        snd_pcm_readi(pcm_handle_c, buffer, frame_size);
        ssize_t sent = sendto(sockfd, buffer, frame_size*channels*sample_size, 0, (struct sockaddr *)&target_addr ,sizeof(target_addr));

        if (sent<0){
            perror("sendto");
        }
    }


    
    snd_pcm_close(pcm_handle_c);
    close(sockfd);


}


void udp_receiver(snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c,snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size){

    
    //playback
    int buffer_size=frame_size * channels * sample_size;
    int16_t *buffer=malloc(buffer_size);
    snd_pcm_hw_params_set_access(pcm_handle_p,params_p,SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle_p,params_p,SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle_p, params_p, channels);
    snd_pcm_hw_params_set_rate(pcm_handle_p,params_p,48000,0);
    snd_pcm_hw_params(pcm_handle_p,params_p);
    snd_pcm_prepare(pcm_handle_p);
    
    
    //UDP_receiver
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("socket");
        exit(1);
    }
    
    struct sockaddr_in recv_addr, sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    recv_addr.sin_family=AF_INET;
    recv_addr.sin_port=htons(5000);
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




void codec_sender(snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c,snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size){
    
    int buffer_size=frame_size * channels * sample_size;
    int16_t *buffer=malloc(buffer_size);

    int sample_rate = 48000; 
    
    //capture
    snd_pcm_hw_params_set_access(pcm_handle_c,params_c,SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle_c,params_c,SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle_c, params_c, channels);
    snd_pcm_hw_params_set_rate(pcm_handle_c,params_c,sample_rate,0);
    snd_pcm_hw_params(pcm_handle_c,params_c);
    snd_pcm_prepare(pcm_handle_c);
    
    //encoder
    int opus_err;
    unsigned char encoded_buffer[4000];
    OpusEncoder *encoder= opus_encoder_create(sample_rate,channels,OPUS_APPLICATION_VOIP,&opus_err);

    
    
    

    //UDP_sender
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd < 0){
        perror("socket");
        exit(1);
        
    }
    
    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(5000); //5000 portu için
    target_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


    while (1){
        snd_pcm_readi(pcm_handle_c, buffer, frame_size);

        int nb_bytes= opus_encode(encoder, buffer, frame_size, encoded_buffer,4000 );

        ssize_t sent = sendto(sockfd, encoded_buffer, nb_bytes, 0, (struct sockaddr *)&target_addr ,sizeof(target_addr));

        if (sent<0){
            perror("sendto");
        }
    }


    
    snd_pcm_close(pcm_handle_c);
    close(sockfd); 
    free(buffer);
    opus_encoder_destroy(encoder);



}


void codec_receiver(snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c,snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size){

    
    //playback
    int buffer_size=frame_size * channels * sample_size;
    int16_t *buffer=malloc(buffer_size);    snd_pcm_hw_params_set_access(pcm_handle_p,params_p,SND_PCM_ACCESS_RW_INTERLEAVED);
    int sample_rate=48000;

    snd_pcm_hw_params_set_format(pcm_handle_p,params_p,SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle_p, params_p, channels);
    snd_pcm_hw_params_set_rate(pcm_handle_p,params_p,sample_rate,0);
    snd_pcm_hw_params(pcm_handle_p,params_p);
    snd_pcm_prepare(pcm_handle_p);
    
    //decoder
    int opus_err;
    int16_t decoded_buffer[frame_size * channels];
    OpusDecoder *decoder= opus_decoder_create(sample_rate,channels,&opus_err);

    
    //UDP_receiver
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0){
        perror("socket");
        exit(1);
    }
    
    struct sockaddr_in recv_addr, sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    recv_addr.sin_family=AF_INET;
    recv_addr.sin_port=htons(5000);
    recv_addr.sin_addr.s_addr=INADDR_ANY ; // local harici için INADDR_ANY local inet_addr("127.0.0.1")

    bind(sockfd, (struct sockaddr*)&recv_addr, sizeof(recv_addr) );

    unsigned char encoded_buffer[4000];


    while (1)
    {

        ssize_t recv_len = recvfrom(sockfd, encoded_buffer, sizeof(encoded_buffer), 0, (struct sockaddr*)&sender_addr,&addr_len);
        
        if(recv_len <0){
            perror("recvfrom");
            continue;
        }else if(recv_len>0){


            int decoded_samples= opus_decode(decoder,encoded_buffer,recv_len,decoded_buffer,frame_size,0);
            
            int err=snd_pcm_writei(pcm_handle_p,decoded_buffer,decoded_samples);

            if(err<0){
                snd_pcm_recover(pcm_handle_p, err, 0);
            }
        }
    }

    
};

