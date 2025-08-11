#ifndef UDP_H
#define UDP_H


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "alsa.h"


void udp_sender(const char *capture, snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c,int frame_size,int channels, int sample_size, int sample_rate,int port ,const char *ip);
void udp_receiver(const char *playback, snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size, int sample_rate, int port);
#endif 