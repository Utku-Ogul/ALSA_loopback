#ifndef deneme_h
#define deneme_h






#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <alsa/asoundlib.h>


typedef struct{
    uint8_t codec_type;
    uint16_t data_length;
    char payload[4000];
} AudioPacket;

#endif deneme_h