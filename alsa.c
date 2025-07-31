#include "alsa.h"

snd_pcm_format_t test_formats[] = {
    SND_PCM_FORMAT_S8,
    SND_PCM_FORMAT_U8,
    SND_PCM_FORMAT_S16_LE,
    SND_PCM_FORMAT_S32_LE,
    SND_PCM_FORMAT_FLOAT_LE,
    SND_PCM_FORMAT_FLOAT64_LE
};


//mikrefon
void print_capture_device_info(const char *device_name) {
    printf("\n\n capture \n\n");
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;

    snd_pcm_open(&pcm_handle,device_name,SND_PCM_STREAM_CAPTURE,0);

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm_handle, params);

    printf("Açilan cihaz: %s\n\n", snd_pcm_name(pcm_handle));

    snd_pcm_info_t *info;
    snd_pcm_info_malloc(&info);
    snd_pcm_info(pcm_handle, info);
    printf("Cihaz ismi: %s \n", snd_pcm_info_get_name(info));


    printf("\nDesteklenen formatlar:\n");
    for (int i = 0; i < 6; i++) {
        if (snd_pcm_hw_params_test_format(pcm_handle, params, test_formats[i]) == 0) {
            printf("- %s\n", snd_pcm_format_name(test_formats[i]));
        }
    }

    unsigned int min_rate, max_rate;
    int dir;
    snd_pcm_hw_params_get_rate_min(params, &min_rate, &dir);
    snd_pcm_hw_params_get_rate_max(params, &max_rate, &dir);
    printf("\nDesteklenen örnekleme hizi:\n- Min: %u Hz\n- Max: %u Hz\n", min_rate, max_rate);

    unsigned int min_channels, max_channels;
    snd_pcm_hw_params_get_channels_min(params, &min_channels);
    snd_pcm_hw_params_get_channels_max(params, &max_channels);
    printf("\nDesteklenen kanal sayisi:\n- Min: %u\n- Max: %u\n", min_channels, max_channels);

    snd_pcm_info_free(info);
    snd_pcm_hw_params_free(params);
    snd_pcm_close(pcm_handle);
}

//hoparlör
void print_playback_device_info(const char *device_name) {
    printf("\n\n playback\n\n");
    snd_pcm_t *pcm_handle;
    snd_pcm_hw_params_t *params;

    snd_pcm_open(&pcm_handle,device_name,SND_PCM_STREAM_PLAYBACK,0);

    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm_handle, params);

    printf("Açilan cihaz: %s\n\n", snd_pcm_name(pcm_handle));

    snd_pcm_info_t *info;
    snd_pcm_info_malloc(&info);
    snd_pcm_info(pcm_handle, info);
    printf("Cihaz ismi: %s \n", snd_pcm_info_get_name(info));

    printf("\nDesteklenen formatlar:\n");
    for (int i = 0; i < 6; i++) {
        if (snd_pcm_hw_params_test_format(pcm_handle, params, test_formats[i]) == 0) {
            printf("- %s\n", snd_pcm_format_name(test_formats[i]));
        }

    }
    unsigned int min_rate, max_rate;
    int dir;
    snd_pcm_hw_params_get_rate_min(params, &min_rate, &dir);
    snd_pcm_hw_params_get_rate_max(params, &max_rate, &dir);
    printf("\nDesteklenen örnekleme hizi:\n- Min: %u Hz\n- Max: %u Hz\n", min_rate, max_rate);

    unsigned int min_channels, max_channels;
    snd_pcm_hw_params_get_channels_min(params, &min_channels);
    snd_pcm_hw_params_get_channels_max(params, &max_channels);
    printf("\nDesteklenen kanal sayisi:\n- Min: %u\n- Max: %u\n", min_channels, max_channels);

    snd_pcm_info_free(info);
    snd_pcm_hw_params_free(params);
    snd_pcm_close(pcm_handle);
}


