#ifndef ALSA_H
#define ALSA_H


#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>



extern snd_pcm_format_t test_formats[];
void print_capture_device_info(const char *device_name);
void print_playback_device_info(const char *device_name);

#endif 