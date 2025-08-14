#include "main.h"

int main(int argc, char *argv[]) {
    const char* capture;
    const char* playback;
    const char* ip;

    snd_pcm_t *pcm_handle_c = NULL, *pcm_handle_p = NULL;
    snd_pcm_hw_params_t *params_c = NULL, *params_p = NULL;
    int frame_size, channels, sample_size, sample_rate, udp_mode, port, receiver_mode;



    if (strcmp(argv[1],"--list-devices") == 0){ 
        list_devices(1);
        printf("\n");
        list_devices(2);

        return 0;

    }else if (strcmp(argv[1], "--device-info") == 0) {
        capture = argv[2];
        playback = argv[3];
        print_capture_device_info(capture, &pcm_handle_c, &params_c);
        print_playback_device_info(playback, &pcm_handle_p, &params_p);

    } else if (strcmp(argv[1], "--loopback") == 0) {
        capture = argv[2];
        playback = argv[3];
        frame_size = atoi(argv[4]);
        channels = atoi(argv[5]);
        sample_size = atoi(argv[6]);
        sample_rate = atoi(argv[7]);
        loopback(capture, playback, &pcm_handle_c, &params_c, &pcm_handle_p, &params_p, frame_size, channels, sample_size, sample_rate);

    } else if (strcmp(argv[1], "--sender") == 0) {
        udp_mode = atoi(argv[2]);
        capture = argv[3];
        frame_size = atoi(argv[4]);
        channels = atoi(argv[5]);
        sample_size = atoi(argv[6]);
        sample_rate = atoi(argv[7]);
        port = atoi(argv[8]);
        ip = argv[9];
        if (udp_mode == 0) {
            udp_sender(capture, pcm_handle_c, params_c, frame_size, channels, sample_size, sample_rate, port, ip);
        } else if (udp_mode == 1) {
            codec_sender(capture, pcm_handle_c, params_c, frame_size, channels, sample_size, sample_rate, port, ip);
        } else {
            printf("mode 0 = udp / mode 1 = codec+udp\n");
        }

    } else if (strcmp(argv[1], "--receiver") == 0) {
        receiver_mode = atoi(argv[2]);
        playback = argv[3];
        frame_size = atoi(argv[4]);
        channels = atoi(argv[5]);
        sample_size = atoi(argv[6]);
        sample_rate = atoi(argv[7]);
        port = atoi(argv[8]);
        if (receiver_mode == 0) {
            automatic_receiver(playback, pcm_handle_p, params_p, frame_size, channels, sample_size, sample_rate, port);
        } else if (receiver_mode == 1) {
            udp_receiver(playback, pcm_handle_p, params_p, channels, sample_size, sample_rate, port);
        } else if (receiver_mode == 2) {
            codec_receiver(playback, pcm_handle_p, params_p, frame_size, channels, sample_size, sample_rate, port);
        } else {
            printf("mode 0 =automatic / mode 1 = udp / mode 2 = codec+udp\n");
        }
    } else {
        printf("unknown command!!!\n");
    }

    snd_pcm_close(pcm_handle_p);
    snd_pcm_close(pcm_handle_c);
    return 0;
}
