#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>



#define FRAME_SIZE 1024
#define CHANNELS 1
#define SAMPLE_SIZE 2



int main(int argc, char *argv[]){
    char buffer[FRAME_SIZE * CHANNELS * SAMPLE_SIZE];
    
    const char* capture=argv[1];
    const char* playback=argv[2];

    snd_pcm_t *pcm_handle_c,*pcm_handle_p;
    snd_pcm_hw_params_t *params_c,*params_p;


    snd_pcm_open(&pcm_handle_c,capture,SND_PCM_STREAM_CAPTURE,0);


    snd_pcm_hw_params_malloc(&params_c);
    
    
    snd_pcm_hw_params_any(pcm_handle_c,params_c);
    
    snd_pcm_hw_params_set_access(pcm_handle_c,params_c,SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle_c,params_c,SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle_c, params_c, CHANNELS);
    snd_pcm_hw_params_set_rate(pcm_handle_c,params_c,48000,0);
    snd_pcm_hw_params(pcm_handle_c,params_c);
    snd_pcm_prepare(pcm_handle_c);
    
    
    snd_pcm_open(&pcm_handle_p,playback,SND_PCM_STREAM_PLAYBACK,0);
    
    snd_pcm_hw_params_malloc(&params_p);
    
    snd_pcm_hw_params_any(pcm_handle_p,params_p);

    snd_pcm_hw_params_set_access(pcm_handle_p,params_p,SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm_handle_p,params_p,SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm_handle_p, params_p, CHANNELS);
    snd_pcm_hw_params_set_rate(pcm_handle_p,params_p,48000,0);
    snd_pcm_hw_params(pcm_handle_p,params_p);
    snd_pcm_prepare(pcm_handle_p);



    while (1) {
        snd_pcm_readi(pcm_handle_c, buffer, FRAME_SIZE);
        snd_pcm_writei(pcm_handle_p, buffer, FRAME_SIZE);
    }

    
    snd_pcm_close(pcm_handle_p);
    snd_pcm_close(pcm_handle_c);

    free(buffer);


}