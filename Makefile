all:
	sudo ./lscTUN &
	sleep 2
	sudo ifconfig lscTUN 10.1.1.11/24

build:
	gcc tunDemo.c -o lscTUN

build-test:
	gcc recive.c -o recive
	gcc send.c -o send

clean:
	rm lscTUN send recive
	-sudo ip link delete lscTUN