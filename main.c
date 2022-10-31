#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <terminos.h>
#include <pthread.h>
#include "main.h"

#define MAX_READ_SIZE 1
#define GPS_MODULE "/dev/ttyUSB1"
//#define CLIENT_CONTROLLER "/dev/ttyUSB0"

int main(int argc, char *argv[]){
    struct uart_Device_struct,gps_device;
    int opt
    
    uart_setup(&gps_device,GPSMODULE,B9600,true);

    //client_contrller_Device.file_name = "/dev/ttyACM0";
    gps_device.file_name = "/dev/ttyUSB0";
    }