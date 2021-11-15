#include<unistd.h>
#include <fcntl.h>  /* O_RDWR */
#include <string.h> /* memset(), memcpy() */
#include <stdio.h> /* perror(), printf(), fprintf() */
#include <stdlib.h> /* exit(), malloc(), free() */
#include <sys/ioctl.h> /* ioctl() */

/* includes for struct ifreq, etc */
#include <sys/types.h>
#include <sys/socket.h>

#include <linux/if_tun.h>
#include<thread>

#include<net/if.h>
#include<linux/if.h>
#include<sys/ioctl.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<linux/if_tun.h>
#include<stdlib.h>
#include<stdio.h>
#include <netdb.h>            // struct addrinfo
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_ICMP, INET_ADDRSTRLEN
#include <netinet/ip.h>       // struct ip and IP_MAXPACKET (which is 65535)
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>


#define TUN_NAME "tun0"
#define TUN_FILE "/dev/net/tun"
#define PHY_INF "ens34"
#define HEADS_LEN 14
#define MAXETHLEN 2000
#define LSC_TYPE 0x09ad




void BindToInterface(int raw , const char *device , int protocol) { 
    struct sockaddr_ll sll;
    struct ifreq ifr; bzero(&sll , sizeof(sll));
    bzero(&ifr , sizeof(ifr)); 
    strncpy((char *)ifr.ifr_name ,device , IFNAMSIZ); 
    //copy device name to ifr 
    if((ioctl(raw , SIOCGIFINDEX , &ifr)) == -1)
    { 
        perror("Unable to find interface index");
        exit(-1); 
    }
    sll.sll_family = PF_PACKET; 
    sll.sll_ifindex = ifr.ifr_ifindex; 
    sll.sll_protocol = protocol; 
    if((bind(raw , (struct sockaddr *)&sll , sizeof(sll))) ==-1)
    {
        perror("bind: ");
        exit(-1);
    }

} 

void eth2tun(int tunFD){
    int n_read;
	unsigned char buffer[MAXETHLEN];
	unsigned char* eth_frame;
	int sock_fd;
	eth_frame = buffer;


    if((sock_fd = socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL))) < 0) // PF_PACKET 收取链路层的数据 ETH_P_ALL 所有的 etherType 都捕获
    {
        printf("error while creating raw socket :%d\n",sock_fd);
        exit(-1);
    }
    BindToInterface(sock_fd, PHY_INF , htons(0x1111));


    while(1)
    {
        n_read = recvfrom(sock_fd, buffer, MAXETHLEN, 0, NULL, NULL);
        if(n_read < 46)
        {
            printf("Eth frame len < 46");
        }
        else{
            n_read = write(tunFD,buffer+HEADS_LEN,n_read-HEADS_LEN); 
        }
        
    }
}



void tun2eth(int tunFD){
    int n_read;
    unsigned char buffer[MAXETHLEN];
    // eth.type
    buffer[12] = 0x11;
    buffer[13] = 0x11;


    // 创建正真发送的socket
    struct sockaddr_ll device;
    memset (&device, 0, sizeof (device));
    if ((device.sll_ifindex = if_nametoindex (PHY_INF)) == 0) {
        perror ("if_nametoindex() failed to obtain interface index ");
        exit (EXIT_FAILURE);
    }
    device.sll_family = AF_PACKET;
    memcpy (device.sll_addr, buffer+6, 6);
    device.sll_halen = htons(6);

    int sd;
    // Submit request for a raw socket descriptor.
    if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {//创建正真发送的socket
        perror ("socket() failed ");
        exit (EXIT_FAILURE);
    }



    while (1){
        n_read = read(tunFD,buffer+HEADS_LEN,sizeof(buffer)); // 读数据
        if (n_read<0){
            printf("remove_submit error\n");
            close(tunFD);
            exit(-1);
        }
        // Send ethernet frame to socket.
        if ((sendto (sd, buffer, n_read+HEADS_LEN, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {
            perror ("sendto() failed");
            exit (EXIT_FAILURE);
        } 

    }
}


int tun_open(char *devname)
{
  struct ifreq ifr;
  int fd, err;

  if ( (fd = open("/dev/net/tun", O_RDWR)) == -1 ) {
       perror("open /dev/net/tun");exit(1);
  }
  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN;
  strncpy(ifr.ifr_name, devname, IFNAMSIZ); // devname = "tun0" or "tun1", etc 

  /* ioctl will use ifr.if_name as the name of TUN 
   * interface to open: "tun0", etc. */
  if ( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) == -1 ) {
    perror("ioctl TUNSETIFF");close(fd);exit(1);
  }

  printf("open tun\n");

  /* After the ioctl call the fd is "connected" to tun device specified
   * by devname ("tun0", "tun1", etc)*/

  return fd;
}

int main(int argc, char *argv[])
{
    int tunFD = tun_open(TUN_NAME);
  std::thread t1(tun2eth,tunFD);
  std::thread t2(eth2tun,tunFD);



  t1.join();
  t1.join();
  return 0;
}
