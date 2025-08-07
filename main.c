#include "main.h"

int main(int argc, char *argv[]) {

    const char* capture=argv[1];
    const char* playback=argv[2];

    snd_pcm_t *pcm_handle_c,*pcm_handle_p;
    snd_pcm_hw_params_t *params_c,*params_p;

    //device info
    print_capture_device_info(pcm_handle_c,params_c);
    print_playback_device_info(pcm_handle_p,params_p);

    //loopback
    //loopback(pcm_handle_c, params_c, pcm_handle_p, params_p,1024, 1, 2);

    // UDP
    //udp_sender(pcm_handle_c, params_c, pcm_handle_p, params_p,1024, 1, 2);
    //udp_receiver(pcm_handle_c, params_c, pcm_handle_p, params_p,1024, 1, 2);

    //opus codec
    codec_sender(pcm_handle_c, params_c, pcm_handle_p, params_p,960, 1, 2);
    //codec_receiver(pcm_handle_c, params_c, pcm_handle_p, params_p,1024, 1, 2);


    snd_pcm_close(pcm_handle_p);
    snd_pcm_close(pcm_handle_c);
    return 0;
}
