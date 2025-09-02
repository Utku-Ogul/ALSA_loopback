#include "ringbuffer.h"

int rb_init(RingBuffer *rb, int cap, int frame_bytes){
    
    rb->cap = cap;
    rb->frame_bytes = frame_bytes;
    rb->r = 0;
    rb->w = 0;

    rb->slots =(uint8_t*)malloc((size_t)cap * (size_t)frame_bytes );






    return 0;
}