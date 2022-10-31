#ifndef GPS_MODULE_H_
#define GPS_MODULE_H_

#include <stdint.h>

#define DEBUG

struct gps_data_structure{
    double latitude;
    double longitude;
    char *gps_time;
    char lat_cardinal_sign;
    char long_cardinal_sign;
    double pdop;
    double vdop;
    double hdop;
    int speed;
};

void *read_from_gps(void *);

void get_lat_log(double *);
void get_dops(char **, char *);
void get_gps_data(char * struct gps_data_struct *);
void get_gps_param_by_position(char **,char*,unit8_t);

#endif
