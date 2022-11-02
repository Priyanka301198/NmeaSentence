#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

int fd = -1;
int end_of_loop= 0;

void sig_handler(int sig)
{
  if(sig == SIGINT)
  {
    printf("GPS parsing stopped by SIGINT\n");
    end_of_loop = 1;
    close(fd);
  }
}



int main(int argc, char *argv[])
{
  struct termios newt;
  char *nmea_line;
  char *parser;
  double latitude;
  float longitude;
  float altitude;

  signal(SIGINT, sig_handler);

  fd = open("/dev/ttyUSB0", O_RDWR | O_NONBLOCK);
  if (fd >= 0)
  {
    tcgetattr(fd, &newt);
    newt.c_iflag &= ~IGNBRK;         
    newt.c_iflag &= ~(IXON | IXOFF | IXANY); 
    newt.c_oflag = 0;               

    newt.c_cflag |= (CLOCAL | CREAD); 
    newt.c_cflag = B9600 | CS8 | CREAD;
    newt.c_cflag |= CS8;                       
    newt.c_cflag &= ~(PARENB | PARODD);         
    newt.c_cflag &= ~CSTOPB;                   

    newt.c_lflag = 0;                            

    newt.c_cc[VMIN]  = 0; 
    newt.c_cc[VTIME] = 0; 
    //cfsetispeed(&newt,9600);
    //cfsetospeed(&newt,9600);
    tcsetattr(fd, TCSANOW, &newt);



  usleep(100000);

  while(end_of_loop == 0)
  {

    char read_buffer[80];
    read(fd, &read_buffer,80);
    //printf("|%s|", r_buf);
    
    printf("Buffer: %s",read_buffer);

    //nmea_line = strtok(read_buffer, "\n");
    nmea_line = read_buffer;
    printf("NMEA line after assigning: %s",nmea_line);

    if (nmea_line != NULL)
    {

      parser = strstr(nmea_line, "$GPRMC");
      if (parser != NULL)
      {
        printf("%s \r\n", nmea_line);
        char *token = strtok(nmea_line, ",");
        int index = 0;
        while (token != NULL)
        {
            if(index==0){
              printf("token: %s\n", token);
                index++;
                continue;
            }
                
          if (index == 3)
          {
            printf("token: %s\n", token);
            latitude = atof(token);
            printf("found latitude: %s %f\n", token, latitude);
            index++;
            continue;
          }
          if (index == 5)
          {
            printf("token: %s\n", token);
            longitude = atof(token);
            printf("found longitude: %s %f\n", token, longitude);
            index++;
            continue;
          }
          //token = strtok(NULL, ",");
          //index++;
        }
      }

      //parser = strstr(nmea_line, "$GPGGA");
      //if (nmea_line[3] == 'G' && nmea_line[4]=='G' && nmea_line[5] == 'A')
      //{
        /*printf("|%s| \n", nmea_line);
        char *token = strtok(nmea_line, ",");
        int index = 0;
        while (token != NULL)
        {
          if (index == 13)
          {
            altitude = atof(token);
            printf("found altitude: %s %f\n", token, altitude);
          }
          token = strtok(NULL, ",");
          index++;
        }
        printf("|%s| \n",latitude);


      }*/



      //printf("|%s| \n", nmea_line);
      //nmea_line = strtok(NULL, "\n");
    }

    usleep(500000);

  }

  close(fd);

  return 0;

  }
  else
  {
    printf("Port cannot be opened");
    return -1;
  }
}
