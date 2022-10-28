#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <errno.h>   /* ERROR Number Definitions           */

#include <pthread.h>

void* gps() {
        int fd;/*File Descriptor*/

                //printf("\n +----------------------------------+");
                //printf("\n |        Serial Port Write         |");
                //printf("\n +----------------------------------+");

                /*------------------------------- Opening the Serial Port -------------------------------*/

                /* Change /dev/ttyUSB0 to the one corresponding to your system */

        fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY); /* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
                                                                        /* O_RDWR Read/Write access to serial port           */
                                                                        /* O_NOCTTY - No terminal will control the process   */
                                                                        /* O_NDELAY -Non Blocking Mode,Does not care about-  */
                                                                        /* -the status of DCD line,Open() returns immediatly */                                        

        if(fd == -1)                                            /* Error Checking */
            printf("\n  Error! in Opening ttyUSB0  ");
        else
            printf("\n  ttyUSB0 Opened Successfully ");


                /*---------- Setting the Attributes of the serial port using termios structure --------- */

        struct termios SerialPortSettings;      /* Create the structure                          */

        tcgetattr(fd, &SerialPortSettings);     /* Get the current attributes of the Serial port */

        cfsetispeed(&SerialPortSettings,B9600); /* Set Read  Speed as 9600                       */
        cfsetospeed(&SerialPortSettings,B9600); /* Set Write Speed as 9600                       */

        SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
        SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
        SerialPortSettings.c_cflag &= ~CSIZE;    /* Clears the mask for setting the data size             */
        SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

        SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
        SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */ 


        SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
        SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

        SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

        if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
            printf("\n  ERROR ! in Setting attributes");
        else
            printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");

                /*------------------------------- Write data to serial port -----------------------------*/

        char write_buffer[] = "A";      /* Buffer containing characters to write into port           */
        unsigned char packet[] = { 0xB5,  0x62, 0x06, // 
        0x08, // 

        0x06, // length

        0x00, // 

        0x64, // measRate, hex 64 = dec 100 ms

        0x00, // 

        0x01, // navRate, always =1

        0x00, // 

        0x01, // timeRef, stick to GPS time (=1)

        0x00, // 

        0x7A, // CK_A

        0x12, // CK_B

    };
    int  bytes_written  = 0;        /* Value for storing the number of bytes written to the port */ 
                //for(int i=0;i<14;i=i+1){
                //      printf(packet[i]);
//}
    bytes_written = write(fd,packet,sizeof(packet));/* use write() to send data to port                                            */
                                                                             /* "fd"                   - file descriptor pointing to the opened serial port */
                                                                             /* "write_buffer"         - address of the buffer containing data              */
                                                                             /* "sizeof(write_buffer)" - No of bytes to write                               */
    char buf [100];
    int n = read (fd, buf, sizeof buf);
    printf(buf);
    printf("\n  %s written to ttyUSB0",write_buffer);
    printf("\n  %d Bytes written to ttyUSB0", bytes_written);
    printf("\n +----------------------------------+\n\n");
    close(fd);/* Close the Serial port */
}

int vcan()
{
	int s, i; 
	int nbyte;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;

	printf("CAN Sockets Receive Demo\r\n");

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {    
		perror("Socket");
		return 1;
	}

	strcpy(ifr.ifr_name, "vcan0" );
	ioctl(s, SIOCGIFINDEX, &ifr);    
	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {  
		perror("Bind");
		return 1;
	}

	nbyte = read(s, &frame, sizeof(struct can_frame));   

 	if (nbyte < 0) {
		perror("Read");
		return 1;
	}

	printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);

	for (i = 0; i < frame.can_dlc; i++)
		printf("%02X ",frame.data[i]);

	printf("\r\n");

	if (close(s) < 0) {
		perror("Close");
		return 1;
	}
	return 0;
}

int main (){
     pthread_t newthread;
     int pthread_create(&newthread,NULL, &gps , NULL);
     //gps();
     vcan();
}
