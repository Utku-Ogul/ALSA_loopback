#ifndef DUPLEX_H
#define DUPLEX_H

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

/* Projendeki mevcut başlıklar */
#include "alsa.h"
#include "udp.h"
#include "udp_codec.h"

/* Basit full-duplex parametreleri */
typedef struct {
    const char *capture;       /* "plughw:1,0" */
    const char *playback;      /* "plughw:2,0" */
    const char *remote_ip;     /* "192.168.1.50" */

    int port_send;             /* uzak cihaza göndereceğin port (örn 5001) */
    int port_listen;           /* yerelde dinleyeceğin port (örn 5000) */

    int frame_size;            /* örn: 960 */
    int channels;              /* 1 veya 2 */
    int sample_size;           /* byte: 1/2/4 (S8/S16/S32) */
    int sample_rate;           /* örn: 48000 */

    int use_codec;             /* 0 = PCM/ham UDP (udp_sender), 1 = Opus (codec_sender) */
} DuplexParams;

/* Struct ile doğrudan başlatma */
int duplex_run(const DuplexParams *p);

/* Argümanlarla başlatma (main için kolaylık):
   Sıra: <capture> <playback> <ip> <port_send> <port_listen> <frame> <ch> <sbytes> <rate> <mode>
   mode: 0=PCM, 1=Opus
   argi: argv içinde bu sıranın başladığı index (genelde 2) */
int duplex_run_from_args(int argc, char **argv, int argi);

#endif
