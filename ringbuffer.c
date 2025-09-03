#include "ringbuffer.h"

int rb_init(RingBuffer *rb, int cap, int frame_bytes){
    
    rb->cap = cap;
    rb->frame_bytes = frame_bytes;
    rb->r = 0;
    rb->w = 0;

    rb->slots =(uint8_t*)malloc((size_t)cap * (size_t)frame_bytes );
    rb->present =(int *)calloc((size_t)cap,sizeof(int));

    return 0;
}

void rb_free(RingBuffer *rb){
    free(rb->slots);
    free(rb->present);
    rb->slots = NULL;
    rb->present= NULL;
    rb->cap = 0;
    rb->frame_bytes = 0;
    rb->r = 0;
    rb->w = 0;
}


int rb_push(RingBuffer *rb, const uint8_t *data){
    if(rb->present[rb->w]){
        return -1; 
    }
    memcpy(rb->slots + (size_t)rb->w * (size_t)rb->frame_bytes, data, (size_t)rb->frame_bytes);
    rb->present[rb->w]=1;
    rb->w=(rb->w+1) % rb->cap;
    return 0;
}

int rb_pop(RingBuffer *rb, uint8_t *out){
    if(!rb->present[rb->r]){
        return -1;
    }

    memcpy(out,rb->slots + (size_t)rb->r * (size_t)rb->frame_bytes, (size_t)rb->frame_bytes);
    rb->present[rb->r]=0;
    rb->r= (rb->r+1) % rb->cap;
    return 0; 
}