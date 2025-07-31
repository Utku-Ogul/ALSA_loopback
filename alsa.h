#ifndef ALSA_H
#define ALSA_H


#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>



extern const snd_pcm_format_t test_formats[];

void print_capture_device_info(snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c);
void print_playback_device_info(snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p);
void loopback(snd_pcm_t *pcm_handle_c, snd_pcm_hw_params_t *params_c,snd_pcm_t *pcm_handle_p, snd_pcm_hw_params_t *params_p,int frame_size,int channels, int sample_size);


#endif 