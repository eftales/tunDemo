# tunDemo
linux下tun收发数据

实验发现，/dev/net/tun 可以被多次打开，每次打开都能会获得不同的文件描述符，不同文件描述符对应不同的 tun 管道

所以需要在只打开一次的情况下，实现接收和发送，这就是为什么需要把接收和发送分配到两个线程的原因

只有用 `sudo ifconfig lscTUN 10.1.2.21/24` 设置之后才能在 ifconfig 中看到这个接口，并且使用这个接口