#include <fcntl.h>  
#include <termios.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

void sleep_ms(int);
int main(void)
{
int fd,i;
if(fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY) < 0)
  {
    fprintf(stdout, "Unable to open serial device: %s\n", strerror(errno));
    return 1;
  }
for(i = 0; i < 500; i++)
 {
  putchar(tcgetattr(fd, &SerialPortSettings));
  fflush(stdout);
 }
fprintf(stdout, "\n");
close(fd);
return 0;
}
void sleep_ms(int milliseconds) 
{
 struct timespec ts;
 ts.tv_sec = milliseconds / 1000;
 ts.tv_nsec = (milliseconds % 1000) * 1000000;
 nanosleep(&ts, NULL);
}



