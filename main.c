#include <stdio.h>
#include "alsa.h"
#include <sys/socket.h>

int main(int argc, char *argv[]) {

    print_capture_device_info(argv[1]);
    print_playback_device_info(argv[2]);


    
    return 0;
}
