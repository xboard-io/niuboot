#include "dm9000x.h"
#include "net.h"
#define NET_TX_BUF 0x5000
#define NET_RX_BUF 0x31180000
#include "utils.h"
#include "init.h"

#define eth_send dm9000_trans
#define eth_recv dm9000_recv

typedef unsigned short n16;
typedef unsigned int n32;
typedef struct {uint8_t b[6];} n48;

typedef struct
{
	n48 dest;
	n48 src;
	uint16_t type;
}HDR_MAC;

typedef struct
{
	uint16_t hw_type;
	uint16_t protocal_type;
	uint8_t mac_addr_len;
	uint8_t ip_addr_len;
	uint16_t action;
	n48 mac_src;
	IPV4 ip_src;
	n48 mac_dest;
	IPV4 ip_dest;
	uint8_t pad[32];
}ARP;

typedef struct
{
	uint8_t head_len;
	uint8_t tos;
	uint16_t ip_len;
	uint16_t id;
	uint16_t frag;
	uint8_t ttl;
	uint8_t type; //icmp 0x1, udp 0x11, tcp 0x6
	uint16_t checksum;
	IPV4 ip_src;
	IPV4 ip_dst;

}HDR_IP;

typedef struct
{
	uint16_t port_src;
	uint16_t port_dest;
	uint16_t udp_len;
	uint16_t checksum;
}HDR_UDP;

typedef	struct
{
	IPV4 ip_src;
	IPV4 ip_dst;
	n16 type;
	n16 len;
}HDR_UDP_PERSUDO;

typedef struct 
{
	HDR_MAC mac;
	ARP	arp;
}TRANS_ARP;

typedef struct
{
	HDR_MAC mac;
	HDR_IP ip;
	HDR_UDP udp;
}TRANS_UDP;

#define NET16(x) (uint16_t)((((uint16_t)(x))>>8) | (((uint16_t)(x))<<8))
#define NET32(x) ( NET16( (uint32_t)(x)>>16 ) | (NET16(x)<<16) )

uint16_t checksum(uint8_t data[], int size, uint32_t sum)
{
	uint16_t word16;
	int i;
	if(sum)
		sum = (~sum)&0xffff;
	if(size&0x1)
	{	//odd number needs to be padded
		data[size] = 0;
	}
    
	// make 16 bit words out of every two adjacent 8 bit words in the packet
	// and add them up
	for (i=0;i<size;i=i+2)
	{
		word16 =((data[i]<<8)&0xFF00)+(data[i+1]&0xFF);
		sum = sum + word16;	
	}
	
	// take only 16 bits out of the 32 bit sum and add up the carries
	while (sum>>16)
	  sum = (sum & 0xFFFF)+(sum >> 16);

	// one's complement the result
	sum = ~sum;
	
return (uint16_t)sum;
}

//#define arp (*((TRANS_ARP*)NET_TX_BUF));
const n48 MADDR_BCAST = {{0xff,0xff,0xff,0xff,0xff,0xff}};
const n48 MADDR_LOCAL = {{0x00,0x1E, 0x68, 0xDF, 0xF9, 0xEE}};

const IPV4 IPADDR_LOCAL = {{192,168,1,250}};
const IPV4 IPADDR_REMOTE = {{192,168,1,100}};

const n16 PORT_LOCAL = NET16(0x1234);
n16 PORT_REMOTE;

const n16 TYPE_ARP = NET16(0x0806);
const n16 TYPE_IP = NET16(0x0800);
const n16 ARP_REQ = NET16(0x0001);
const n16 ARP_ACK = NET16(0x0002);

n48 MADDR_REMOTE;
int ping_ok;

void ping_callback(char *buf, int size)
{
	TRANS_ARP* ack = (TRANS_ARP*)buf;
	printf("get a package buf %x,size %x\n",buf , size);
	if(memcmp(&ack->mac.dest , &MADDR_LOCAL , sizeof MADDR_LOCAL)==0)
		if(memcmp(&ack->arp.ip_src , &IPADDR_REMOTE , sizeof IPADDR_REMOTE)==0)
			if(ack->arp.action == ARP_ACK) 
			{
				ping_ok = ~0;
				MADDR_REMOTE = ack->mac.src;
			}
			//	if(mem
			//}
}
/*return value: 
 	0 stands for sucessful
	non-zero stands for missing
*/
int ping(IPV4 ip)
{
	int i;
	TRANS_ARP* data = (TRANS_ARP*)NET_TX_BUF;
	memset(data ,0, sizeof(TRANS_ARP));
	data->mac.dest = MADDR_BCAST;
	data->mac.src = MADDR_LOCAL;
	data->mac.type = TYPE_ARP;
	data->arp.hw_type = NET16(0x0001);
	data->arp.protocal_type = NET16(0x0800);
	data->arp.mac_addr_len = 6;
	data->arp.ip_addr_len = 4;
	data->arp.action = ARP_REQ;
	data->arp.mac_dest= MADDR_BCAST;
	data->arp.mac_src= MADDR_LOCAL;
	data->arp.ip_dest= ip;//IPADDR_REMOTE;
	data->arp.ip_src= IPADDR_LOCAL;
//	for(;;)
//	{
	eth_send((char*)data, sizeof(TRANS_ARP));
	ping_ok = 0;
	for(i=0;i<10;i++)
	{	
		udelay(100000);
		eth_recv(ping_callback);
		if(ping_ok)
		{
			printf("ping ok\n");
			break; 
		}
	}
	return (i==10);
}

char *recv_udp_buf;
int recv_udp_len;

void send_udp_callback(char *buf, int size)
{
	TRANS_UDP* ack = (TRANS_UDP*)buf;
	if(memcmp(&ack->mac.dest , &MADDR_LOCAL , sizeof MADDR_LOCAL)==0)
		if(memcmp(&ack->ip.ip_src , &IPADDR_REMOTE , sizeof IPADDR_REMOTE)==0)
			if(ack->udp.port_dest == PORT_LOCAL) 
			{
				PORT_REMOTE = ack->udp.port_src;
				recv_udp_buf = buf+sizeof(TRANS_UDP);
				recv_udp_len = NET16(ack->udp.udp_len) - sizeof ack->udp;
			}	
}
char* send_udp(IPV4 ip, uint16_t port, char* buf, int size, int* recv_size)
{
	int i;
	TRANS_UDP* data = (TRANS_UDP*)NET_TX_BUF;
	memset(data ,0, sizeof(TRANS_UDP));
	//if(ping(ip))
		//return 0;
	memcpy( (char*)NET_TX_BUF + sizeof(TRANS_UDP), buf, size); 
	data->mac.dest = MADDR_REMOTE;
	data->mac.src = MADDR_LOCAL;
	data->mac.type = TYPE_IP;
	data->ip.head_len = 0x45;
	data->ip.tos = 0;
	data->ip.ip_len = NET16(size + sizeof data->ip + sizeof data->udp);
	data->ip.id = NET16(0x3322);
	data->ip.frag = 0;
	data->ip.ttl = 128;
	data->ip.type = 0x11; //icmp 0x1, udp 0x11, tcp 0x6
	data->ip.ip_src = IPADDR_LOCAL;
	data->ip.ip_dst =ip;
	data->ip.checksum = NET16( checksum((uint8_t*)&data->ip, sizeof(data->ip), 0) );
	data->udp.port_src = PORT_LOCAL;
	data->udp.port_dest = port?NET16(port):PORT_REMOTE;
	data->udp.udp_len = NET16(size + sizeof(data->udp));
	data->udp.checksum =  checksum((uint8_t*)&data->udp, sizeof data->udp + size, 0) ;
	//udp persudo-header addition cs
	{
		HDR_UDP_PERSUDO hup;
		hup.ip_src = data->ip.ip_src;
		hup.ip_dst = data->ip.ip_dst;
		hup.type = NET16(0x0011);
		hup.len = data->udp.udp_len;
		data->udp.checksum = NET16(checksum((uint8_t*)&hup, sizeof(hup), data->udp.checksum));
	}
	eth_send((char*)data, sizeof(TRANS_UDP)+size);
	recv_udp_buf = 0;
	recv_udp_len = 0;
	for(i=0;i<10;i++)
	{
		udelay(100000);
		eth_recv(send_udp_callback);
		if(recv_udp_len)
		{
			*recv_size = recv_udp_len;
			return recv_udp_buf;
		}			
	}
	return 0;
}

char* strlink(char *dst, char* src)
{
	//assume dst space big enough
	do
	{
		*dst++=*src;
	}while(*src++);

	return dst;
}
