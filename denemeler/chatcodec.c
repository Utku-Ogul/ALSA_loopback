#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <opus/opus.h>

#define FRAME_SIZE 160 // 20ms @ 8000 Hz
#define SAMPLE_RATE 8000
#define CHANNELS 1
#define MAX_PACKET_SIZE 4000

int main() {
    int err;

    // ALSA
    snd_pcm_t *pcm_capture, *pcm_playback;
    snd_pcm_hw_params_t *params;
    snd_pcm_hw_params_alloca(&params);

    // ALSA: capture aygıtını aç
    err = snd_pcm_open(&pcm_capture, "plughw:1,0", SND_PCM_STREAM_CAPTURE, 0);
    if (err < 0) {
        fprintf(stderr, "Capture open error: %s\n", snd_strerror(err));
        return 1;
    }

    // ALSA: playback aygıtını aç
    err = snd_pcm_open(&pcm_playback, "plughw:2,0", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        fprintf(stderr, "Playback open error: %s\n", snd_strerror(err));
        return 1;
    }

    // Ortak ALSA ayarları (hem capture hem playback için)
    for (int i = 0; i < 2; i++) {
        snd_pcm_t *handle = (i == 0) ? pcm_capture : pcm_playback;
        snd_pcm_hw_params_any(handle, params);
        snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
        snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
        snd_pcm_hw_params_set_channels(handle, params, CHANNELS);
        snd_pcm_hw_params_set_rate(handle, params, SAMPLE_RATE, 0);
        snd_pcm_hw_params(handle, params);
        snd_pcm_prepare(handle);
    }

    // Opus encoder
    int opus_err;
    OpusEncoder *encoder = opus_encoder_create(SAMPLE_RATE, CHANNELS, OPUS_APPLICATION_VOIP, &opus_err);
    if (opus_err != OPUS_OK) {
        fprintf(stderr, "Opus encoder error: %s\n", opus_strerror(opus_err));
        return 1;
    }

    // Opus decoder
    OpusDecoder *decoder = opus_decoder_create(SAMPLE_RATE, CHANNELS, &opus_err);
    if (opus_err != OPUS_OK) {
        fprintf(stderr, "Opus decoder error: %s\n", opus_strerror(opus_err));
        return 1;
    }

    // Buffers
    opus_int16 in_pcm[FRAME_SIZE];
    unsigned char encoded[MAX_PACKET_SIZE];
    opus_int16 out_pcm[FRAME_SIZE];

    printf("Başlatıldı. Ctrl+C ile çıkabilirsiniz.\n");

    while (1) {
        // 1. Capture
        err = snd_pcm_readi(pcm_capture, in_pcm, FRAME_SIZE);
        if (err < 0) {
            fprintf(stderr, "Capture error: %s\n", snd_strerror(err));
            snd_pcm_prepare(pcm_capture);
            continue;
        }

        // 2. Encode
        int nb_bytes = opus_encode(encoder, in_pcm, FRAME_SIZE, encoded, MAX_PACKET_SIZE);
        if (nb_bytes < 0) {
            fprintf(stderr, "Opus encode error: %s\n", opus_strerror(nb_bytes));
            continue;
        }

        // 3. Decode
        int frame_decoded = opus_decode(decoder, encoded, nb_bytes, out_pcm, FRAME_SIZE, 0);
        if (frame_decoded < 0) {
            fprintf(stderr, "Opus decode error: %s\n", opus_strerror(frame_decoded));
            continue;
        }

        // 4. Playback
        err = snd_pcm_writei(pcm_playback, out_pcm, frame_decoded);
        if (err < 0) {
            fprintf(stderr, "Playback error: %s\n", snd_strerror(err));
            snd_pcm_prepare(pcm_playback);
        }
    }

    // Temizlik
    opus_encoder_destroy(encoder);
    opus_decoder_destroy(decoder);
    snd_pcm_close(pcm_capture);
    snd_pcm_close(pcm_playback);

    return 0;
}
