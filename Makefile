all:
	sudo ./lscTUN &
	sleep 2
	sudo ifconfig lscTUN 10.1.1.11/24

build:
	gcc tunDemo.c -o lscTUN

build-test:
	gcc trans.c -o trans -lpthread

clean:
	rm lscTUN trans
	-sudo ip link delete lscTUN