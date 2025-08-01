#include <stdio.h>
#include "alsa.h"
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {

    const char* capture=argv[1];
    const char* playback=argv[2];

    snd_pcm_t *pcm_handle_c,*pcm_handle_p;
    snd_pcm_hw_params_t *params_c,*params_p;

    //capture
    snd_pcm_open(&pcm_handle_c,capture,SND_PCM_STREAM_CAPTURE,0);
    snd_pcm_hw_params_malloc(&params_c);
    snd_pcm_hw_params_any(pcm_handle_c,params_c);


    //playback
    snd_pcm_open(&pcm_handle_p,playback,SND_PCM_STREAM_PLAYBACK,0);
    snd_pcm_hw_params_malloc(&params_p);
    snd_pcm_hw_params_any(pcm_handle_p,params_p);

    //device info
    print_capture_device_info(pcm_handle_c,params_c);
    print_playback_device_info(pcm_handle_p,params_p);

    //loopback
    //loopback(pcm_handle_c, params_c, pcm_handle_p, params_p,1024, 1, 2);

    // UDP
    udp_sender(pcm_handle_c, params_c, pcm_handle_p, params_p,1024, 1, 2);



    snd_pcm_close(pcm_handle_p);
    snd_pcm_close(pcm_handle_c);
    return 0;
}
