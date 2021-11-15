build:
	g++ basic.cpp -o basic -std=c++11 -lpthread

test:
	ping 10.0.0.2 &  # leave this running in another shell to be able to see the effect of the next example
	sudo ./basic

create-tun:
	sudo ip tuntap add mode tun dev tun0
	sudo ip addr add 10.0.0.1/24 dev tun0  # give it an ip
	sudo ip link set dev tun0 up  # bring the if up
	
clean:
	-sudo ip tuntap del mode tun dev tun0
	-rm basic
