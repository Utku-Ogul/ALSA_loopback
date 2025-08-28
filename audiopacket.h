#ifndef AUDIO_PACKET_H
#define AUDIO_PACKET_H
#include <stdint.h>


typedef struct{
    uint16_t frame_size;
    uint16_t channels;
    uint16_t sample_size;    // network order (byte/channel)
    uint32_t sample_rate;
    uint32_t seq;
    uint8_t codec_type;
    uint16_t data_length;
    char payload[4000];
} AudioPacket;


#endif
