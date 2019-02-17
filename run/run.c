/*
 * Created by IntelliJ IDEA.
 * Project: LED-Gpio-Interrupt-Linux-Device-Driver
 * ===========================================
 * User: ByeongGil Jung
 * Date: 2019-02-12
 * Time: 오후 5:12
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

int main(int argc, char **argv) {
    char input[10];
    int dev;
    int switch_state = 0;  // off
    int buff[1024];

    printf("Device driver test.\n");

    dev = open("/dev/my_device_interrupt", O_RDWR);
    printf("dev = %d\n", dev);

    while(1) {
        printf("Please put the button\n");
        printf("0 : exit, 1 : on/off LED\n");
        scanf("%s", input);

        if (*input == '0') {
            break;
        } else if (*input == '1') {
            if (switch_state == 0) {
                ioctl(dev, 0, 1);
                switch_state = 1;

                printf("LED on\n");
            } else {
                ioctl(dev, 0, 0);
                switch_state = 0;

                printf("LED off\n");
            }
        }
    }

    close(dev);

    exit(EXIT_SUCCESS);
}
