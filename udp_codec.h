#ifndef CODEC_H
#define CODEC_H


#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <opus/opus.h>
#include "alsa.h"
#include "udp.h"
#include "audiopacket.h"
#include <stddef.h> 



void  codec_sender(const char *capture,snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c, int frame_size,int channels, int sample_size, int sample_rate, int port,const char *ip);
int codec_receiver(const char *playback, snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size,int sample_rate,int port );
void automatic_receiver(const char *playback, snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size,int sample_rate,int port );


#endif 