#ifndef JITTER_H
#define JITTER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


typedef struct {
    int frame_ms;       /* örn: 20 */
    int frame_bytes;    /* örn: 960 * ch * bytes_per_sample */
    int cap_frames;     /* ring kapasitesi, örn: 128 */
    int target_frames;  /* prefill/hedef gecikme, örn: 2 (40 ms) */
} JitterParams;

typedef struct {
    /* Ring */
    uint8_t *slots;     /* cap * frame_bytes */
    uint8_t *present;   /* cap uzunlukta 0/1 dizisi */
    int cap;
    int frame_bytes;
    int frame_ms;

    /* Zaman/Sıra */
    uint32_t next_play_seq;
    uint64_t next_play_ms;
    int started;        /* prefill tamam mı? */

    /* Sayaçlar (loss/reorder/dup/late) */
    uint32_t base_seq, max_seq, received, lost, dup, reorder, late_drop;
} JitterBuf;




#endif