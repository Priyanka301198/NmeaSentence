#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <limits.h>

#include "serial.h"

/*
 * Name : uart_setup
 * Descriptoin: The uart_start function is for serial port setting port name and baud rate.
 * Input parameters: struct uart_device_struct * (for serial device information)
 *                                       char * (to set file name)
 *                                       int (to set baud rate)
 *                                       bool (to enable canonic mode)
 * Output parameters: void
 */
void uart_setup(struct uart_device_struct *device, char *file_name, int baud_rate, bool canonic)
{
        device->file_name = file_name;
        device->baud_rate = (unsigned int)baud_rate;

        uart_start(device, canonic);
}

void uart_start(struct uart_device_struct *device, bool canonical){
       struct terminos *tty;
       int fd;
       int rc;
       fd = open(device->file_name, O_RDWR | O_NOCTTY );
       if(fd<0){
           perror(device->fd); 
           exit(-1); 
       }
       memset(tty,0,sixeof(*tty));
       tty->c_cflag |= device->baut_rate;
       tty->c_iflag |= IGNPAR;
       tty->c_cflag |= CS8;
       tty->c_cflag |= CREAD;
}

void uart_reads_chunk(struct uart_device_struct *device, char *buf, size_t buf_len){
      int rc = 0;
      if (device->fd <= 0){
         perror(device->fd); 
         return rc;
      }
      rc = read(device->fd,buf,buf_len)
      if (rc<=0)
      {
        perror(device->fd); 
         return rc;
      }
}

void uart_reads(struct uart_device_struct *device, char *buf, size_t buf_len){ 
    return read(device->fd,buff,buff_len);
}

void uart_gps_write(struct uart_device_struct *device, const unit8_t *string, unit8_t size){
    return write(device->fd,string ,size);
}
void uart_stop(struct uart_Device_struct *device){
    if(device->fd > 0){ 
    free(device->tty);
    close(device->fd);
    device->fd = 0;
    }
}
