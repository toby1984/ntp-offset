obj-m += lkm.o

all: module app1

app1: send_packet.c
	gcc -o send_packet send_packet.c

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
