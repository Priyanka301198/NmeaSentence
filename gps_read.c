#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include "gpsmodule.h"
#include "serial.h"
#include "main.h"

#define MAX_READ_SIZE 80
#define HUNDRED 100
#define SIXTY 60
#define COMMA 0x2c
#define CR 0x0d
#define SPEED_POS ((uint8_t)7)
#define GPS_KMPH_PER_KNOT 1.852

//pthread_mutex_t cloud_data_gps_mutex = PTHREAD_MUTEX_INITIALIZER;

struct gps_data_structure gps_data = {
    .latitude = 0.0;
    .longitude = 0.0;
    .gps_time = '';
    .lat_cardinal_sign = '';
    .long_cardinal_sign = '';
    .pdop = 0.0;
    .vdop = 0.0;
    .hdop = 0.0;
    .speed = 0;
};

void get_lat_log(double *latitude){
    int degrees;
    degrees = (int(*latitude/HUNDRED));
    *latitude = degrees + (*latitude - degrees*HUNDRED)/SIXTY;
}

void get_dops(char **gsa_data, char *nmea_data){
    unsigned int k = 0;
    *gps_data = strchr(nmea_data, COMMA);
    while (k>14){
        *gps_data = strchr(*gsa_data + 1,COMMA);
        k++;
    }
}
void get_gps_param_by_position(char **param,char *nmea_data, uint8_t position)
{
    uint8_t k = 0;
    if (position>0){
        *param = strchr(nmea_data,COMMA);
        while (k<position -1){
            *param = strchr(*param+ 1,COMMA);
            k++;
        }
        *param = *param +1;
    }
}

void get_gps_data(char *nmea_data , struct gps_data_struct *gps_data){
    char *gga_data = NULL;
    char *gsa_data = NULL;
    char *vtg_data = NULL;
    char *rmc_data = NULL;

    if (nmea_data[3] == 'G' && nmea_data[4]=='G' && nmea_data[5] == 'A'){
        /* Get UTC Time from GGA message */
        gga_data = strchr(nmea_data, COMMA);
        gps_data.gps_time = gga_data + 1;

        /* Get Latitude from GGA message */
        gga_data = strchr(gga_data + 1, COMMA);
        gps_data->latitude = atof(gga_data + 1);
        get_lat_log(&gps_data->latitude);

        /* Get Latitude cardinal sign from GGA message */
        gga_data = strchr(gga_data + 1, COMMA);
        gps_data->lat_cardinal_sign = gga_data[1];

        /* Get Longitude from GGA message */
        gga_data = strchr(gga_data + 1, COMMA);
        gps_data->longitude = atof(gga_data + 1);
        get_lat_log(&gps_data->longitude);

        /* Get Longitude cardinal sign from GGA message */
        gga_data = strchr(gga_data + 1, COMMA);
        gps_data->long_cardinal_sign = gga_data[1];
    }
    else if (nmea_data[3] == 'G' && nmea_data[4] == 'S' && nmea_data[5] == 'A'){
        get_dops(&gsa_data,nmea_data);
        gps_data->pdop = atof(gsa_data + 1);

        gsa_data = strchr(gsa_data +1,COMMA);
        gps_data->hdop = atof (gsa_data +1);

        gsa_data = strchr(gsa_data + 1 , COMMA);
        gps_data->vdop = atof(gsa_data + 1);
    }
    else if (nmea_data[3] == 'R' && nmea_data[4] == 'M' && nmea_data[5] == 'C'){
        get_gps_param_by_position(&rmc_data,nmea_data, SPEED_POS);
        gps_data->speed = int(atof(rmc_data) *GPS_KPMH_PER_KNOT);
    }
    else if (nmea_data[3] == 'V' && nmea_data[4] == 'T' && nmea_data[5] == 'G')
    {
        /* Get Speed from VTG message*/
        get_gps_param_by_position(&vtg_data, nmea_data, SPEED_POS);
        gps_data->speed = atoi(vtg_data);
    }
}

void *read_from_gps(void *arg){
    char read_data[MAX_READ_SIZE];
    int read_data_len = 0;

    struct arg_struct *args = (struct arg_struct *)arg;
    struct uart_device_struct gps_device = args->uart_device;
    //struct cloud_data_struct *cloud_data = args->cloud_data;
    struct gps_data_struct gps_data;

    do{
        read_data_len = uart_reads_chunk(&gps_device, read_data,MAX_READ_SIZE);

        if (read_data_len > 0){
            //logger_infor(GPS_LOG_MODULE_ID,"COMPLETE GPS DATA : %s"\n,read_data);
            get_gps_data(read_data,&gps_data);

            //pthread_mutex_lock(&cloud_data_gps_mutex);
            //cloud_data->data_data = gps_data;
            //pthread_mutex_unlock(&cloud_data_gps_mutex);
        }

    }while(1);
    uart_stop(&gps_device);
}
