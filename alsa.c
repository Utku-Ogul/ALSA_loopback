#include "alsa.h"
#include "udp.h"

const snd_pcm_format_t test_formats[] = {
    SND_PCM_FORMAT_S8,
    SND_PCM_FORMAT_U8,
    SND_PCM_FORMAT_S16_LE,
    SND_PCM_FORMAT_S32_LE,
    SND_PCM_FORMAT_FLOAT_LE,
    SND_PCM_FORMAT_FLOAT64_LE
};

int open_capture_device(const char *capture, snd_pcm_t **pcm_handle_c, snd_pcm_hw_params_t **params_c,int channels, int sample_rate){

    //capture

    snd_pcm_open(pcm_handle_c,capture,SND_PCM_STREAM_CAPTURE,0);
    snd_pcm_hw_params_malloc(params_c);
    snd_pcm_hw_params_any(*pcm_handle_c,*params_c);
    snd_pcm_hw_params_set_access(*pcm_handle_c,*params_c,SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(*pcm_handle_c,*params_c,SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(*pcm_handle_c, *params_c, channels);
    snd_pcm_hw_params_set_rate(*pcm_handle_c,*params_c,sample_rate,0);
    snd_pcm_hw_params(*pcm_handle_c,*params_c);
    snd_pcm_prepare(*pcm_handle_c);
    return 0;
}


int open_playback_device(const char *playback,snd_pcm_t **pcm_handle_p, snd_pcm_hw_params_t **params_p, int channels, int sample_rate){


    //playback
    snd_pcm_open(pcm_handle_p,playback,SND_PCM_STREAM_PLAYBACK,0);
    snd_pcm_hw_params_malloc(params_p);
    snd_pcm_hw_params_any(*pcm_handle_p,*params_p);
    snd_pcm_hw_params_set_access(*pcm_handle_p,*params_p,SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(*pcm_handle_p,*params_p,SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(*pcm_handle_p, *params_p, channels);
    snd_pcm_hw_params_set_rate(*pcm_handle_p,*params_p,sample_rate,0);
    snd_pcm_hw_params(*pcm_handle_p,*params_p);
    snd_pcm_prepare(*pcm_handle_p);
    return 0;
}



//mikrefon
void print_capture_device_info(const char capture, snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c) {
    snd_pcm_open(pcm_handle_c,capture,SND_PCM_STREAM_CAPTURE,0);
    snd_pcm_hw_params_malloc(params_c);
    snd_pcm_hw_params_any(pcm_handle_c,params_c);
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
void print_playback_device_info(const char *playback,snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p) {
    snd_pcm_open(pcm_handle_p,playback,SND_PCM_STREAM_PLAYBACK,0);
    snd_pcm_hw_params_malloc(params_p);
    snd_pcm_hw_params_any(pcm_handle_p,params_p);
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




void loopback(const char *capture,const char *playback ,snd_pcm_t **pcm_handle_c, snd_pcm_hw_params_t **params_c,snd_pcm_t **pcm_handle_p, snd_pcm_hw_params_t **params_p,int frame_size,int channels,int sample_size, int sample_rate ){

    int buffer_size=frame_size * channels * sample_size;
    int16_t *buffer=malloc(buffer_size);

    //capture
    if (open_capture_device(capture, pcm_handle_c, params_c, channels, sample_rate)!=0){
        fprintf(stderr,"loopback-open_capture_device!!!");
        return;
    };

    
    //playback
    if(open_playback_device(playback, pcm_handle_p, params_p, channels, sample_rate)!=0){
        fprintf(stderr, "loopback-open_playback_device!!!");
        return;
    };



    while (1) {
        snd_pcm_readi(*pcm_handle_c, buffer, frame_size);
        snd_pcm_writei(*pcm_handle_p, buffer, frame_size);
    };


    free(buffer);

}



