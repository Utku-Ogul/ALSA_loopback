#ifndef CODEC_H
#define CODEC_H


#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <opus/opus.h>

typedef struct{
    uint8_t codec_type;
    uint16_t data_length;
    char payload[4000];
} AudioPacket;


#endif 