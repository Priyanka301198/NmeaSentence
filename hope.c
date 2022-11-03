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
    newt.c_cflag |= CS8;   
    newt.c_cflag |= B9600;
    newt.c_cflag &= ~(PARENB | PARODD);         
    newt.c_cflag &= ~CSTOPB;                   

    newt.c_lflag = 0;                            

    newt.c_cc[VMIN]  = 0; 
    newt.c_cc[VTIME] = 0; 
    tcsetattr(fd, TCSANOW, &newt);



  usleep(100000);

  while(end_of_loop == 0)
  {

    char read_buffer[80];
    read(fd, &read_buffer,80);
    //printf("|%s|", r_buf);
    printf("\n%s\n",read_buffer);
    nmea_line = strtok(read_buffer, "$");
    //nmea_line = read_buffer;
    printf("NMEA:\n%s",nmea_line);
    while (nmea_line != NULL)
    {
      if (nmea_line[3] == 'G' && nmea_line[4]=='G' && nmea_line[5] == 'A')
    {
      char utc_time = strchr(nmea_line,",");
      float time = utc_time + 1;
      printf("Found Time %f",time);

      char latitude = strchr(utc_time+1,",");
      float lat = utc_time + 1;
      printf("Found Latitude %f",lat);

      /*lat_card = strchr(utc_time+1,",");
      if (lat_card[1]=='S' || lat_card[1]=='s'){

      }*/
    }
      /*parser = strstr(nmea_line, "$GPGGA");
      if (parser != NULL)
      {
        //printf("%s \r\n", nmea_line);
        char *token = strtok(nmea_line, ",");
        int index = 0;
        while (token != NULL)
        {
            if(index==0){
                index++;
                continue;
            }
                
          if (index == 1)
          {
            latitude = atof(token);
            printf("found latitude: %s %f\n", token, latitude);
            index++;
            continue;
          }
          if (index == 2)
          {
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
