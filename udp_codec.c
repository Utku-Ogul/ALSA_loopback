#include "udp_codec.h"

void  codec_sender(const char *capture,snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c, int frame_size,int channels, int sample_size, int sample_rate, int port,const char *ip){
    
    int buffer_size=frame_size * channels * sample_size;
    int16_t *buffer=malloc(buffer_size);
    
    //capture
    if (open_capture_device(capture, &pcm_handle_c, &params_c, channels, sample_rate)!=0){
        fprintf(stderr,"codec-open_capture_device!!!");
        return;
    };
    
    //encoder
    AudioPacket packet;
    packet.codec_type=1; //0=> udp 1=>opus+udp

    packet.frame_size  = htons(frame_size);
    packet.channels    = htons(channels);
    packet.sample_size = htons(sample_size);
    packet.sample_rate = htonl(sample_rate);





    int opus_err;
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
    target_addr.sin_port = htons(port); //5000 portu için
    target_addr.sin_addr.s_addr = inet_addr(ip);


    while (1){
        snd_pcm_readi(pcm_handle_c, buffer, frame_size);


        int nbytes= opus_encode(encoder, buffer, frame_size, (unsigned char *)packet.payload, 4000);
        



        packet.data_length = htons((uint16_t)nbytes);

        ssize_t sent = sendto(sockfd, &packet , offsetof(AudioPacket, payload)+nbytes, 0, (struct sockaddr *)&target_addr ,sizeof(target_addr));

        if (sent<0){
            perror("sendto");
        }
    }


    
    snd_pcm_close(pcm_handle_c);
    close(sockfd); 
    free(buffer);
    opus_encoder_destroy(encoder);
}


void codec_receiver(const char *playback, snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size,int sample_rate,int port ){

    AudioPacket packet;
    
    
    if(open_playback_device(playback, &pcm_handle_p, &params_p, channels, sample_rate)!=0){
        fprintf(stderr, "codec-open_playback_device!!!");
    };
    
    snd_pcm_hw_params_set_access(pcm_handle_p,params_p,SND_PCM_ACCESS_RW_INTERLEAVED);
    
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

    memset(&recv_addr, 0, sizeof(recv_addr)); 
    recv_addr.sin_family=AF_INET;
    recv_addr.sin_port=htons(port);
    recv_addr.sin_addr.s_addr=htonl(INADDR_ANY) ; // local harici için INADDR_ANY local inet_addr("127.0.0.1")

    bind(sockfd, (struct sockaddr*)&recv_addr, sizeof(recv_addr) );

    while (1)
    {

        ssize_t recv_len = recvfrom(sockfd,&packet, sizeof(packet), 0, (struct sockaddr*)&sender_addr,&addr_len);

        
        if(recv_len <0){
            perror("recvfrom");
            continue;
        }else if(recv_len>0){
            uint16_t payload_len = ntohs(packet.data_length);
            int decoded_samples= opus_decode(decoder,(unsigned char*)packet.payload ,payload_len,decoded_buffer,frame_size,0);
            
            int err=snd_pcm_writei(pcm_handle_p,decoded_buffer,decoded_samples);

            if(err<0){
                snd_pcm_recover(pcm_handle_p, err, 0);
            }
        }
    }
    
};

void automatic_receiver(const char *playback, snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p, int frame_size, int channels, int sample_size, int sample_rate,int port)
{

    AudioPacket packet;
    
    
    if(open_playback_device(playback, &pcm_handle_p, &params_p, channels, sample_rate)!=0){
        fprintf(stderr, "codec-open_playback_device!!!");
    };
    
    snd_pcm_hw_params_set_access(pcm_handle_p,params_p,SND_PCM_ACCESS_RW_INTERLEAVED);
    
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

    memset(&recv_addr, 0, sizeof(recv_addr)); 
    recv_addr.sin_family=AF_INET;
    recv_addr.sin_port=htons(port);
    recv_addr.sin_addr.s_addr=htonl(INADDR_ANY) ; // local harici için INADDR_ANY local inet_addr("127.0.0.1")

    bind(sockfd, (struct sockaddr*)&recv_addr, sizeof(recv_addr) );

    while (1)
    {

        ssize_t recv_len = recvfrom(sockfd,&packet, sizeof(packet), 0, (struct sockaddr*)&sender_addr,&addr_len);
        
        if(recv_len <0){
            perror("recvfrom");
            continue;
        }else if(recv_len>0){

            uint16_t payload_len = ntohs(packet.data_length);

            if(packet.codec_type==1){

                int decoded_samples= opus_decode(decoder,(unsigned char*)packet.payload ,payload_len,decoded_buffer,frame_size,0);
                
                int err=snd_pcm_writei(pcm_handle_p,decoded_buffer,decoded_samples);
    
                if(err<0){
                    snd_pcm_recover(pcm_handle_p, err, 0);
                }
            }else if(packet.codec_type==0){
                int frames = payload_len / (channels * sample_size);
                if (frames > 0) {
                    int err = snd_pcm_writei(pcm_handle_p, packet.payload, frames);
                    if (err < 0) snd_pcm_recover(pcm_handle_p, err, 0);
                }
            }
        }
    }
}

void full_automatic_receiver(const char *playback,
                             snd_pcm_t *pcm_handle_p,
                             snd_pcm_hw_params_t *params_p,
                             int port)
{
    AudioPacket packet;

    // UDP
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); return; }

    struct sockaddr_in recv_addr, sender_addr;
    socklen_t addr_len = sizeof(sender_addr);
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family      = AF_INET;
    recv_addr.sin_port        = htons(port);
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr*)&recv_addr, sizeof(recv_addr)) < 0) {
        perror("bind");
        close(sockfd);
        return;
    }

    // Akım durumu
    int configured       = 0;
    int cur_rate         = 0;
    int cur_channels     = 0;
    int cur_sample_size  = 0;     // bytes per sample
    int cur_frame        = 0;     // samples per channel
    uint8_t cur_codec    = 0xFF;  // bilinmiyor

    // Opus
    OpusDecoder *opus_dec = NULL;
    int opus_err = 0;

    // Decode buffer (dinamik)
    int16_t *decoded_buffer = NULL;
    size_t   decoded_cap    = 0;  // toplam int16 örnek kapasitesi (samples*channels)

    for (;;) {
        ssize_t recv_len = recvfrom(sockfd, &packet, sizeof(packet), 0,
                                    (struct sockaddr*)&sender_addr, &addr_len);
        if (recv_len < 0) {
            perror("recvfrom");
            continue;
        }
        if (recv_len == 0) continue;

        // Başlığı network byte order'dan çevir
        uint16_t hdr_frame       = ntohs(packet.frame_size);
        uint16_t hdr_channels    = ntohs(packet.channels);
        uint16_t hdr_sample_size = ntohs(packet.sample_size);
        uint32_t hdr_rate        = ntohl(packet.sample_rate);
        uint8_t  hdr_codec       = packet.codec_type;
        uint16_t payload_len     = ntohs(packet.data_length);

        // Konfig değişimi var mı?
        int need_reconf = !configured ||
                          hdr_codec        != cur_codec     ||
                          (int)hdr_rate    != cur_rate      ||
                          (int)hdr_channels!= cur_channels  ||
                          (int)hdr_frame   != cur_frame     ||
                          (int)hdr_sample_size != cur_sample_size;

        if (need_reconf) {
            // Eski paket kuyruklarını boşalt (lag/robotik önler)
            for (;;) {
                ssize_t n = recvfrom(sockfd, &packet, sizeof(packet),
                                     MSG_DONTWAIT, (struct sockaddr*)&sender_addr, &addr_len);
                if (n <= 0) break;
            }

            // ALSA playback’i yeni rate/channels ile güvenli şekilde yeniden aç
            if (pcm_handle_p) {
                snd_pcm_drop(pcm_handle_p);
                snd_pcm_close(pcm_handle_p);
                pcm_handle_p = NULL;
            }
            if (open_playback_device(playback, &pcm_handle_p, &params_p,
                                     (int)hdr_channels, (int)hdr_rate) != 0) {
                fprintf(stderr, "open_playback_device failed\n");
                configured = 0;
                continue;
            }

            // Opus decoder’ı yeniden kur / kapat
            if (opus_dec) { opus_decoder_destroy(opus_dec); opus_dec = NULL; }
            if (hdr_codec == 1) {
                opus_dec = opus_decoder_create((int)hdr_rate, (int)hdr_channels, &opus_err);
                if (opus_err != OPUS_OK) {
                    fprintf(stderr, "opus_decoder_create: %d\n", opus_err);
                    continue;
                }
            }

            // Decode buffer kapasitesi
            size_t need = (size_t)hdr_frame * (size_t)hdr_channels;
            if (need > decoded_cap) {
                int16_t *tmp = (int16_t*)realloc(decoded_buffer, need * sizeof(int16_t));
                if (!tmp) { fprintf(stderr, "realloc failed\n"); continue; }
                decoded_buffer = tmp;
                decoded_cap    = need;
            }

            // Yeni durumu kaydet + bu paketi atla (temiz başlangıç)
            cur_codec       = hdr_codec;
            cur_rate        = (int)hdr_rate;
            cur_channels    = (int)hdr_channels;
            cur_frame       = (int)hdr_frame;
            cur_sample_size = (int)hdr_sample_size;
            configured      = 1;
            continue;
        }

        // Veri oynatma
        if (cur_codec == 0) {
            // PCM (ham)
            int frames = (cur_channels > 0 && cur_sample_size > 0)
                         ? (int)(payload_len / (cur_channels * cur_sample_size))
                         : 0;
            if (frames > 0) {
                snd_pcm_sframes_t w = snd_pcm_writei(pcm_handle_p,
                                                     (const int16_t*)packet.payload,
                                                     frames);
                if (w < 0) snd_pcm_recover(pcm_handle_p, w, 0);
            }
        } else if (cur_codec == 1) {
            // OPUS
            if (!opus_dec) continue;
            if ((size_t)(cur_frame * cur_channels) > decoded_cap) {
                int16_t *tmp = (int16_t*)realloc(decoded_buffer,
                                (size_t)cur_frame * (size_t)cur_channels * sizeof(int16_t));
                if (!tmp) { fprintf(stderr, "realloc failed\n"); continue; }
                decoded_buffer = tmp;
                decoded_cap    = (size_t)cur_frame * (size_t)cur_channels;
            }

            int decoded = opus_decode(opus_dec,
                                      (const unsigned char*)packet.payload,
                                      payload_len,
                                      decoded_buffer,
                                      cur_frame, /*decode_fec=*/0);
            if (decoded < 0) {
                fprintf(stderr, "opus_decode err: %d\n", decoded);
                snd_pcm_prepare(pcm_handle_p); // hızlı toparlama
                continue;
            }

            snd_pcm_sframes_t w = snd_pcm_writei(pcm_handle_p, decoded_buffer, decoded);
            if (w < 0) snd_pcm_recover(pcm_handle_p, w, 0);
        } else {
            // bilinmeyen codec -> atla
        }
    }

    // (Ulaşılmayabilir)
    if (opus_dec) opus_decoder_destroy(opus_dec);
    free(decoded_buffer);
    close(sockfd);
}