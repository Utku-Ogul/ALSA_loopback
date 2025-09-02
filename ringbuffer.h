#ifndef RINGBUFFER_H
#define RINGBUFFER_H


#include <stdio.h>
#include <stdint.h>
#include <stddef.h> 


typedef struct {
    uint8_t *slots;   // cap * frame_bytes kadar ham buffer
    int     *present; // her slot için 0/1 işaret
    int cap;          // toplam slot sayısı (kaç frame saklayabiliyor)
    int frame_bytes;  // tek frame’in byte cinsinden uzunluğu
    int r;            // okuma index → tüketici (ALSAnın okuyacağı yer)
    int w;            // yazma index → üretici (UDP’den gelen veriyi koyduğu yer)
} RingBuffer;

int rb_init(RingBuffer *rb, int cap, int frame_bytes);

#endif 