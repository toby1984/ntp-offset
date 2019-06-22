obj-m += crudepps.o

all: module test_app

test_app: fake_pps_packet.c
	gcc -o fake_pps_packet fake_pps_packet.c

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f fake_pps_packet 2>&1 >/dev/null
