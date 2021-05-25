#include<net/if.h>
#include<sys/ioctl.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<linux/if_tun.h>
#include<stdlib.h>
#include<stdio.h>


#include <errno.h> 
#include <arpa/inet.h>

#define TUN_NAME "lscTUN"
#define TUN_FILE "/dev/net/tun"

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


int main(){
    int tun_fd,n_read,n=3;
    char buffer[3000];

    tun_fd = tun_create(IFF_TUN|IFF_NO_PI);

    if (tun_fd<0){
        printf("create tun/tap device error\n");
        return -1;
    }

    // 设置ip
    // ifconfig lscTUN 10.1.1.11/24 # 在输入这个命令之后，才会在 ifconfig 列表中看到

    while (n>0){
        n -= 1;
        n_read = read(tun_fd,buffer,sizeof(buffer)); // 读数据
        if (n_read<0){
            printf("recive error\n");
            close(tun_fd);
            return -1;
        }

        // n_read = write(tun_fd,buffer,sizeof(buffer)); // 写数据

        printf("Read %d bytes\n",n_read);


    }
    close(tun_fd);
    return 0;
}
