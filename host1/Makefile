build:
	g++ demo.cpp -o demo -std=c++11 -lpthread

test:
	ping 10.0.0.2 &  # leave this running in another shell to be able to see the effect of the next example
	sudo ./demo

create-tun:
	sudo ip tuntap add mode tun dev tun0
	sudo ip addr add 10.0.0.1/24 dev tun0  # give it an ip
	sudo ip link set dev tun0 up  # bring the if up
	sudo ip link set dev ens34 up  # bring the if up

clean:
	-sudo ip tuntap del mode tun dev tun0
	-rm demo
