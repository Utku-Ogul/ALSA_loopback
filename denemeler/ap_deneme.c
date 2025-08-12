#include "ap_deneme.h"
#include <stdint.h>
int main (){


    #define DEVICE "plughw:1,0"
    #define RATE 48000
    #define CHANNELS 1
    #define FORMAT SND_PCM_FORMAT_S16_LE
    #define FRAMES 960


    snd_pcm_t *capture_handle, *playback_handle;
    snd_pcm_hw_params_t *hw_params ,*hw_params1;

    int err;
    if ((err = snd_pcm_open(&capture_handle, DEVICE, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "Capture open error: %s\n", snd_strerror(err));
        return 1;
    }

    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params, FORMAT);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, CHANNELS);
    snd_pcm_hw_params_set_rate(capture_handle, hw_params, RATE, 0);
    snd_pcm_hw_params(capture_handle, hw_params);
    snd_pcm_prepare(capture_handle);


    AudioPacket packet;

    packet.codec_type=1;


    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(5000); //5000 portu için
    target_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


    const size_t h_len =sizeof(packet.codec_type) + sizeof(packet.data_length);

    while ( 1)
    {
        snd_pcm_readi(capture_handle,packet.payload,packet.data_length);
    }
    





    return 0;
}