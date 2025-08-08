#ifndef ALSA_H
#define ALSA_H


#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <opus/opus.h>



extern const snd_pcm_format_t test_formats[];

int open_capture_device(const char *capture, snd_pcm_t **pcm_handle_c, snd_pcm_hw_params_t **params_c,int channels, int sample_rate);
int open_playback_device(const char *playback,snd_pcm_t **pcm_handle_p, snd_pcm_hw_params_t **params_p, int channels, int sample_rate);
void print_capture_device_info(const char capture, snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c);
void print_playback_device_info(const char *playback,snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p);
void loopback(const char *capture,const char *playback ,snd_pcm_t **pcm_handle_c, snd_pcm_hw_params_t **params_c,snd_pcm_t **pcm_handle_p, snd_pcm_hw_params_t **params_p,int frame_size,int channels,int sample_rate ,int sample_size);



#endif 