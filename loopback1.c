#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#define DEVICE "default"
#define RATE 16000
#define CHANNELS 1
#define FORMAT SND_PCM_FORMAT_S16_LE
#define FRAMES 1024

int main() {
    snd_pcm_t *capture_handle, *playback_handle;
    snd_pcm_hw_params_t *hw_params;
    int err;
    char *buffer;
    int buffer_size = FRAMES * CHANNELS * 2; // 2 bytes per sample (S16_LE)

    buffer = (char *)malloc(buffer_size);
    if (!buffer) {
        fprintf(stderr, "Buffer allocation failed\n");
        return 1;
    }

    // Capture cihazı aç
    if ((err = snd_pcm_open(&capture_handle, DEVICE, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "Capture open error: %s\n", snd_strerror(err));
        return 1;
    }

    // Playback cihazı aç
    if ((err = snd_pcm_open(&playback_handle, DEVICE, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        fprintf(stderr, "Playback open error: %s\n", snd_strerror(err));
        return 1;
    }

    // Capture ayarları
    snd_pcm_hw_params_malloc(&hw_params);
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params, FORMAT);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, CHANNELS);
    snd_pcm_hw_params_set_rate(capture_handle, hw_params, RATE, 0);
    snd_pcm_hw_params(capture_handle, hw_params);

    // Playback ayarları
    snd_pcm_hw_params_any(playback_handle, hw_params);
    snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(playback_handle, hw_params, FORMAT);
    snd_pcm_hw_params_set_channels(playback_handle, hw_params, CHANNELS);
    snd_pcm_hw_params_set_rate(playback_handle, hw_params, RATE, 0);
    snd_pcm_hw_params(playback_handle, hw_params);

    snd_pcm_hw_params_free(hw_params);

    snd_pcm_prepare(capture_handle);
    snd_pcm_prepare(playback_handle);

    printf("Başlatıldı... CTRL+C ile çık.\n");

    while (1) {
        if ((err = snd_pcm_readi(capture_handle, buffer, FRAMES)) < 0) {
            snd_pcm_recover(capture_handle, err, 0);
        }

        if ((err = snd_pcm_writei(playback_handle, buffer, FRAMES)) < 0) {
            snd_pcm_recover(playback_handle, err, 0);
        }
    }

    snd_pcm_close(capture_handle);
    snd_pcm_close(playback_handle);
    free(buffer);
    return 0;
}
