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


