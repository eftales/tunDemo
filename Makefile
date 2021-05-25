all:
	sudo ./lscTUN &
	ifconfig lscTUN 10.1.1.11/24

build:
	gcc tunDemo.c -o lscTUN

clean:
	rm lscTUN