## mptcp-netlink-pm-api-example
* This is an example of the MPTCP Netlink API tested.
* Example to toggle the priorities of two Subflows.
* A demo video at the bottom.

<br>

## Environments
#### Server
<pre>
HW
	- Desktop PC (x86)
	- One Ethernet NIC
OS
	- Ubuntu 18.04.x
	- Kernel 4.19.x
	- Mptcp 0.95 (mptcp of multipath-tcp.org)
Package
	- libnl3
</pre>
#### Client
<pre>
HW
	- Raspberry Pi 3 B+ (Arm)
	- Two Ethernet NIC
OS
	- Raspberry Pi OS 2020-05-xx
	- Kernel 4.19.x
	- Mptcp 0.95 (mptcp of multipath-tcp.org)
Package
	- libnl3
	- ifstat (for NIC statistics)
	- speedometer (for NIC statistics)
</pre>  

<br>

## Caution
* Must be an MPTCP for multipath-tcp.org.
* The mptcp version must be 0.95 or higher
* The client must have at least two NICs.
* The client must complete Routing setup. (See this link : https://multipath-tcp.org/pmwiki.php/Users/ConfigureRouting)
* Servers and clients must not use loopback addresses.

<br>

## Installation Package
<pre>
libnl3 (Must)

$ sudo apt-get install libnl-3-dev libnl-genl-3-dev
</pre>
<pre>
ifstat (NIC statistics)

$ sudo apt-get install ifstat
</pre>
<pre>
speedometer (NIC statistics)

$ sudo apt-get install speedometer
</pre>

<br>

## How to installations?
<pre>
$ cd src
$ make
</pre>

<br>

## How to run?
##### in server
<pre>
$ cd src
$ sudo ./mptcp_nl_server [ip_address] [port_number]
</pre>
##### in client
<pre>
$ cd src
$ sudo ./mptcp_nl_client [file_path]
</pre>

<br>

## Demo video
https://user-images.githubusercontent.com/48320014/130901292-cc24f3ab-1e45-41c4-a870-5ad56d4cf949.mp4
* I recommend you watch it on full screen.


