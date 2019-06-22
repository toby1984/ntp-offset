# ntp-offset
A custom Linux PPS (pulse-per-second) source that listens for special UDP broadcast packets send by an Arduino with Ethernet Shield + DCF77 receiver

This is a project I want to (ab-)use to have multiple NTP servers running in sync with different (but constant) offsets. My use-case is a virtual machine host that runs many VMs that all perform operations at the start of each minute. Since this obviously leads to crazy load spikes every minute, I want to setup 4 different NTP servers that are 15 seconds apart from each other and assign the guest VMs evenly to those NTP servers. In the end, the peak load on the VM host should thus only be a quarter of what it is right now.

Intentionally running an NTP server with a fixed offset is obviously something that is "slightly" outside of the original design. So I came up with the following idea:

1. Run the NTP servers on separate machines, using the local clock as their primary time source BUT also using a PPS signal to advance the local clock.
2. Configure the local clocks on these servers to be 15 seconds apart
3. Use an Arduino + Ethernet Shield + DCF77 receiver to generate special UDP broadcast packets every second
4. Use a custom Linux kernel module to register a netfilter hook that listens for these UDP packets and also have this module register a custom PPS source that we can use to discipline the local clocks

## Requirements (Linux Kernel Module)

This project was developed on Ubuntu 18.04.2 LTS with Kernel 4.15.0-51.
You'll need to usual suspects installed (on Debian/Ubuntu this means at least the packages build-essential and kernel sources/headers). It's helpful to also install the 'pps-tools' package on the NTP host as well.

## Installation (Linux NTP host)

### Setting up the kernel module 

Run 

    make install
    
to compile *AND* load the 'crudepps' kernel module. You will still need to make preparations to have this module loaded every time you boot the machine (for example by creating a /etc/modules-load.d/crudepps.conf file and putting 'crudepps' in it).


### Setting up ntpd

TODO

    
  
