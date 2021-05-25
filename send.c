#include<net/if.h>
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


#include <errno.h> 
#include <arpa/inet.h>

#define TUN_NAME "lscTUN"
#define TUN_FILE "/dev/net/tun"
#define PHY_INF "ens34"
#define HEADS_LEN 22

int tun_create(int flags){
    struct ifreq ifr;
    int fd,err;
    char *dev = TUN_FILE;
    if ((fd = open(dev,O_RDWR))<0){
        printf("open %s filed\n",TUN_FILE);
        return fd;
    }
    memset(&ifr,0,sizeof(ifr));
    ifr.ifr_flags = flags;
    strncpy(ifr.ifr_name,TUN_NAME,IF_NAMESIZE);
    err = ioctl(fd,TUNSETIFF,(void*)&ifr);
    if (err<0){
        printf("set name filed\n");
        close(fd);
        return err;
    }

    printf("tun/tap device : %s is up\n",TUN_NAME);
    return fd;
}

int generateFrame(unsigned char* frame){
    for (int i=0;i<12;++i){
        frame[i] = i;
    }
    frame[12] = 0x34;
    frame[13] = 0x12;

}

int addHeads(unsigned char* frame,unsigned char* data){
    memcpy(frame+HEADS_LEN,data,sizeof(data));
    return 0;

}

void send_frame(char* interface,unsigned char* frame,int len){
    struct sockaddr_ll device;
    memset (&device, 0, sizeof (device));
    if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {
        perror ("if_nametoindex() failed to obtain interface index ");
        exit (EXIT_FAILURE);
    }
    device.sll_family = AF_PACKET;
    memcpy (device.sll_addr, frame+6, 6);
    device.sll_halen = htons (6);


    int sd;
    // Submit request for a raw socket descriptor.
    if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {//创建正真发送的socket
        perror ("socket() failed ");
        exit (EXIT_FAILURE);
    }
    // Send ethernet frame to socket.
    if ((sendto (sd, frame, len, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {
        perror ("sendto() failed");
        exit (EXIT_FAILURE);
    }
    printf ("sended.\n");     
    close (sd);
}

int main(){
    int tun_fd,n_read;
    unsigned char buffer[1500];
    unsigned char frame[3000];
    generateFrame(frame);

    tun_fd = tun_create(IFF_TUN|IFF_NO_PI);

    if (tun_fd<0){
        printf("create tun/tap device error\n");
        return -1;
    }

    // 设置ip
    // ifconfig lscTUN 10.1.1.11/24 # 在输入这个命令之后，才会在 ifconfig 列表中看到

    while (1){
        n_read = read(tun_fd,buffer,sizeof(buffer)); // 读数据
        if (n_read<0){
            printf("recive error\n");
            close(tun_fd);
            return -1;
        }
        // n_read = write(tun_fd,buffer,sizeof(buffer)); // 写数据;只能写网络层的数据，写以太帧的话会报错
        printf("Read %d bytes\n",n_read);

        addHeads(frame,buffer);
        send_frame(PHY_INF,frame,n_read+HEADS_LEN);

        printf("write to PHY_INF\n");

    }
    close(tun_fd);
    return 0;
}
