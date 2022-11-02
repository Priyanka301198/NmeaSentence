#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#define maskId          0x1FFFFFFFU //Data

int main(int argc, char **argv)
{
	int s, i; 
	int nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	struct can_frame frameRequest;
	struct timeval tv;
	const uint8_T obd2PID;
	const uint8_T obd2Mode;
	uint32_T Identifier;
	uint8_T ReturnedBytes;
	u_int8_t Mode[];
	u_int8_t *PID[];
	u_int8_t *A;
	u_int8_t *B;
	u_int8_t *C;
	u_int8_t *D;
	real_T Timestamp[];
	uint8_t RawData[];
	uint32_t NewMessage[];
	const real_T sampleTime;
        const int p_width0;
	int *receivedBytes;
        uint32_t msgIDRaw;
        uint32_t msgID;

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

    struct can_filter rfilter[1];

	rfilter[0].can_id   = 0x7DF;
	rfilter[0].can_mask = 0xFF0;
	//rfilter[1].can_id   = 0x200;
	//rfilter[1].can_mask = 0x700;

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    nbytes = read(s, &frame, sizeof(struct can_frame));

	if (nbytes < 0) {
		perror("Read");
		return 1;
	}

	printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);

	for (i = 0; i < frame.can_dlc; i++)
		printf("%02X ",frame.data[i]);

	printf("\r\n");

    frameRequest.can_id = 0x7DF;
    frameRequest.can_dlc = 8;
    frameRequest.data[0] = 2;
    frameRequest.data[1] = obd2Mode[0];
    frameRequest.data[2] = obd2PID[0];
    frameRequest.data[3] = 0;
    frameRequest.data[4] = 0;
    frameRequest.data[5] = 0;
    frameRequest.data[6] = 0;
    frameRequest.data[7] = 0;

	//sprintf(frame.data, "Hello");

	if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("Write");
		return 1;
	}

	receivedBytes = read(s, &frame, sizeof(struct can_frame));

	if (receivedBytes != sizeof(struct can_frame) ) {
        NewMessage[0] = 0;
    }else{
        
        while (receivedBytes == sizeof(struct can_frame)) {
            msgIDRaw    = (uint32_T)frame.can_id; 
            if (msgIDRaw >= 0x7E8 && msgIDRaw <= 0x7EF) 
			{
                ioctl(s, SIOCGSTAMP, &tv);      
                msgID       = msgIDRaw & maskId;    

                Identifier[0]   = msgID;
                Timestamp[0]    = tv.tv_sec + tv.tv_usec * 1e-6;
                NewMessage[0]   = 1;
                
                ReturnedBytes[0] = (uint8_T)frame.data[0];
                Mode[0] = (u_int8_t)frame.data[1];
                PID[0] = (u_int8_t)frame.data[2];
                A[0] = (u_int8_t)frame.data[3];
                B[0] = (u_int8_t)frame.data[4];
                C[0] = (u_int8_t)frame.data[5];
                D[0] = (u_int8_t)frame.data[6];
                
                for(i=0; i<8; i++) {
                    RawData[i] = (uint8_T)frame.data[i];
                }  
            } 
            // check again
            receivedBytes = read(s, &frame, sizeof(struct can_frame));
        }
    }

    if (close(s) < 0) {
		perror("Close");
		return 1;
	}

	return 0;
}
