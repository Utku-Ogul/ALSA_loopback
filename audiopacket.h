#ifndef AUDIO_PACKET_H
#define AUDIO_PACKET_H
#include <stdint.h>


typedef struct{
    uint8_t codec_type;
    uint16_t data_length;
    char payload[4000];
} AudioPacket;


#endif
