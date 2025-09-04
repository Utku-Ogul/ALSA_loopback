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
    const char *capture;       
    const char *playback;      
    const char *remote_ip;     

    int port_send;           
    int port_listen;           

    int frame_size;           
    int channels;              
    int sample_size;           
    int sample_rate;        

    int use_codec;             
} DuplexParams;

int duplex_run(const DuplexParams *p);

/* Argümanlarla başlatma (main için kolaylık):
   Sıra: <capture> <playback> <ip> <port_send> <port_listen> <frame> <ch> <sbytes> <rate> <mode>
   mode: 0=PCM, 1=Opus
   argi: argv içinde bu sıranın başladığı index (genelde 2) */
int duplex_run_from_args(int argc, char **argv, int argi);

#endif
