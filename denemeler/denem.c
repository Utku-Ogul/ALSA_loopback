#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// ---------------- WAV Recorder ----------------
typedef struct {
    FILE     *fp;
    uint32_t  data_bytes;
    uint16_t  channels;
    uint32_t  sample_rate;
    uint16_t  bits_per_sample;
} Recorder;

#pragma pack(push, 1)
typedef struct {
    char     riff_id[4];      // "RIFF"
    uint32_t riff_size;       // 36 + data_bytes
    char     wave_id[4];      // "WAVE"
    char     fmt_id[4];       // "fmt "
    uint32_t fmt_size;        // 16
    uint16_t audio_format;    // 1 = PCM
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char     data_id[4];      // "data"
    uint32_t data_size;
} WavHeader;
#pragma pack(pop)

int recorder_open(Recorder *r, const char *path,
                  uint16_t channels, uint32_t sample_rate, uint16_t bits_per_sample) {
    r->fp = fopen(path, "wb");
    if (!r->fp) return -1;
    r->channels = channels;
    r->sample_rate = sample_rate;
    r->bits_per_sample = bits_per_sample;
    r->data_bytes = 0;

    // Geçici header yaz
    WavHeader h;
    memcpy(h.riff_id, "RIFF", 4);
    h.riff_size = 36; // placeholder
    memcpy(h.wave_id, "WAVE", 4);
    memcpy(h.fmt_id, "fmt ", 4);
    h.fmt_size        = 16;
    h.audio_format    = 1;
    h.num_channels    = channels;
    h.sample_rate     = sample_rate;
    uint16_t block_align = (channels * bits_per_sample) / 8;
    h.block_align     = block_align;
    h.bits_per_sample = bits_per_sample;
    h.byte_rate       = sample_rate * block_align;
    memcpy(h.data_id, "data", 4);
    h.data_size       = 0;

    fwrite(&h, sizeof(h), 1, r->fp);
    return 0;
}

int recorder_write(Recorder *r, const void *pcm, size_t bytes) {
    if (!r->fp) return -1;
    size_t w = fwrite(pcm, 1, bytes, r->fp);
    r->data_bytes += (uint32_t)w;
    return (w == bytes) ? 0 : -1;
}

int recorder_close(Recorder *r) {
    if (!r->fp) return -1;
    long end_pos = ftell(r->fp);
    uint32_t data_size = r->data_bytes;
    uint32_t riff_size = 36 + data_size;

    fseek(r->fp, 4, SEEK_SET);
    fwrite(&riff_size, sizeof(riff_size), 1, r->fp);

    fseek(r->fp, 40, SEEK_SET);
    fwrite(&data_size, sizeof(data_size), 1, r->fp);

    fseek(r->fp, end_pos, SEEK_SET);
    fclose(r->fp);
    r->fp = NULL;
    return 0;
}

// ---------------- Test Main ----------------
int main(void) {
    Recorder rec;
    const char *outfile = "deneme.wav";

    if (recorder_open(&rec, outfile, 1, 48000, 16) != 0) {
        fprintf(stderr, "Recorder açılamadı!\n");
        return 1;
    }

    // 1 saniye sessizlik (48000 örnek, 1 kanal, 16-bit = 96000 byte)
    int samples = 48000;
    int16_t *buffer = calloc(samples, sizeof(int16_t));
    if (!buffer) {
        perror("malloc");
        return 1;
    }

    recorder_write(&rec, buffer, samples * sizeof(int16_t));
    recorder_close(&rec);
    free(buffer);

    printf("WAV dosyası kaydedildi: %s\n", outfile);
    return 0;
}
