#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <time.h>

void sleep_ms(int);
int main(void)
{
int fd,i;
if((fd = serialOpen("/dev/ttyUSB0", 9600)) < 0)
  {
    fprintf(stdout, "Unable to open serial device: %s\n", strerror(errno));
    return 1;
  }
for(i = 0; i < 500; i++)
 {
  putchar(serialGetchar(fd));
  fflush(stdout);
 }
fprintf(stdout, "\n");
serialClose(fd);
return 0;
}
void sleep_ms(int milliseconds) 
{
 struct timespec ts;
 ts.tv_sec = milliseconds / 1000;
 ts.tv_nsec = (milliseconds % 1000) * 1000000;
 nanosleep(&ts, NULL);
}

