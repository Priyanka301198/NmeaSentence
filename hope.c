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

#define COMMA 0x2C

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
    
   // while (nmea_line != NULL)
    {
      //printf("\nNMEA:\n%s",nmea_line);
      char *utc_time = NULL;
     // printf("\nNMEA Sentence from GPS module %s\n", nmea_line);
      char *test_nmea = "$GPGGA,053125.00,1731.99230,N,07830.46644,E,1,05,2.46,414.3,M,-73.9,M,,*76";
      printf("\n%s\n",test_nmea);
      if (test_nmea[3] == 'G' && test_nmea[4]=='G' && test_nmea[5] == 'A')
     {
      utc_time = strchr(test_nmea,COMMA);
      char *time = (utc_time + 1);
      //printf("Found Time %s\n",time);

      char *latitude = strchr(utc_time+1,COMMA);
      float lat = atof(latitude + 1);
      printf("Found Latitude %f\n",lat);
        
      char *longitude = strchr(latitude+2,COMMA);
      float longi = atof(longitude + 2);
      printf("Found longitude %f\n",longi);

      /*lat_card = strchr(utc_time+1,",");
      if (lat_card[1]=='S' || lat_card[1]=='s'){

      }*/
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
      }*/

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
      nmea_line = strtok(NULL, "$");
    }

    usleep(500000);

  }

  close(fd);
  
  return 0;

  }
  }
  else
  {
    printf("Port cannot be opened");
    return -1;
  }
}
