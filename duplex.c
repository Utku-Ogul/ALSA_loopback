#include "duplex.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* Projendeki mevcut başlıklar */
#include "alsa.h"
#include "udp.h"
#include "udp_codec.h"

/* ---------------- Thread hedefleri ---------------- */

typedef struct {
    /* TX tarafı için gerekli alanlar */
    const char *capture;
    const char *ip;
    int port_send;
    int frame_size, channels, sample_size, sample_rate;
    int use_codec; /* 0=PCM/udp_sender, 1=Opus/codec_sender */
} TxArgs;

typedef struct {
    /* RX tarafı için gerekli alanlar */
    const char *playback;
    int port_listen;
} RxArgs;

/* Mic -> UDP (PCM veya Opus) */
static void *tx_thread(void *vp) {
    TxArgs *a = (TxArgs*)vp;

    /* Bu sender fonksiyonlar senin projende ALSA’yı içeride açıyor.
       Handle/params NULL geçilebilir. */
    snd_pcm_t *pcm_c = NULL;
    snd_pcm_hw_params_t *params_c = NULL;

    if (a->use_codec == 1) {
        /* Opus ile gönder */
        codec_sender(a->capture, pcm_c, params_c,
                     a->frame_size, a->channels, a->sample_size, a->sample_rate,
                     a->port_send, a->ip);
    } else {
        /* Ham PCM ile gönder */
        udp_sender(a->capture, pcm_c, params_c,
                   a->frame_size, a->channels, a->sample_size, a->sample_rate,
                   a->port_send, a->ip);
    }

    return NULL;
}

/* UDP -> Hoparlör (senin full_automatic_receiver) */
static void *rx_thread(void *vp) {
    RxArgs *a = (RxArgs*)vp;

    /* full_automatic_receiver içeride konfigürasyon paketine göre
       ALSA açıp (gerekirse format/rate/ch/frame dinamik) çalıyor. */
    snd_pcm_t *pcm_p = NULL;
    snd_pcm_hw_params_t *params_p = NULL;

    full_automatic_receiver(a->playback, pcm_p, params_p, a->port_listen);
    return NULL;
}

/* ---------------- Public API ---------------- */

int duplex_run(const DuplexParams *p) {
    if (!p) {
        fprintf(stderr, "duplex_run: params NULL\n");
        return -1;
    }

    TxArgs tx = {
        .capture     = p->capture,
        .ip          = p->remote_ip,
        .port_send   = p->port_send,
        .frame_size  = p->frame_size,
        .channels    = p->channels,
        .sample_size = p->sample_size,
        .sample_rate = p->sample_rate,
        .use_codec   = p->use_codec
    };

    RxArgs rx = {
        .playback    = p->playback,
        .port_listen = p->port_listen
    };

    pthread_t th_tx, th_rx;

    if (pthread_create(&th_tx, NULL, tx_thread, &tx) != 0) {
        perror("pthread_create(tx)");
        return -1;
    }
    if (pthread_create(&th_rx, NULL, rx_thread, &rx) != 0) {
        perror("pthread_create(rx)");
        return -1;
    }

    /* Basit kullanım: ana thread ikisini bekler (Ctrl+C ile süreç sonlanır). */
    pthread_join(th_tx, NULL);
    pthread_join(th_rx, NULL);
    return 0;
}

int duplex_run_from_args(int argc, char **argv, int i) {
    /* <cap> <play> <ip> <port_send> <port_listen> <frame> <ch> <sbytes> <rate> <mode> */
    if (i + 10 > argc) {
        fprintf(stderr, "duplex_run_from_args: eksik argüman\n");
        return -1;
    }

    DuplexParams p;
    p.capture     = argv[i + 0];
    p.playback    = argv[i + 1];
    p.remote_ip   = argv[i + 2];
    p.port_send   = atoi(argv[i + 3]);
    p.port_listen = atoi(argv[i + 4]);
    p.frame_size  = atoi(argv[i + 5]);
    p.channels    = atoi(argv[i + 6]);
    p.sample_size = atoi(argv[i + 7]);
    p.sample_rate = atoi(argv[i + 8]);
    p.use_codec   = atoi(argv[i + 9]); /* 0=PCM, 1=Opus */

    return duplex_run(&p);
}
