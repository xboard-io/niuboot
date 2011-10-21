 
typedef struct{char b[4];} IPV4;

typedef struct
{
	unsigned short op;
	union
	{
		char filename[1];
		unsigned short blocknum;
	}u;
	char data[1];
}HDR_TFTP;

char* send_udp(IPV4 ip, unsigned short port, char* buf, int size, int* recv_size);
int ping(IPV4 ip);
char* strlink(char *dst, char* src);
 

