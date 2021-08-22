## Brief
* 간략한 설명 (to Eng)

<br>

## Directory Tree
* src  
ㄴ client : client source  
ㄴ server : server source  
ㄴ func : common function  
ㄴ hdr : general header   

<br>

## Environments
* **Server**
	* HW
		* Desktop PC (x86)
		* One Ethernet NIC
	* OS
		* Ubuntu 18.04.x
		* Kernel 4.19.x
		* Mptcp 0.95 (mptcp of multipath-tcp.org)
	* Package
		* libnl3
<br>

* **Client**
	* HW
		* Raspberry Pi 3 B+ (Arm)
		* Two Ethernet NIC
	* OS
		* Raspberry Pi OS 2020-05-xx
		* Kernel 4.19.x
		* Mptcp 0.95 (mptcp of multipath-tcp.org)
	* Package
		* libnl3
		* ifstat (for testing)

<br>

## Installation Package
<pre>
libnl3

$ sudo apt-get install libnl-3-dev libnl-genl-3-dev
</pre>

<pre>
ifstat

$ sudo apt-get install ifstat
</pre>

<br>

## Caution
* Servers and clients must not use loopback addresses.
* The mptcp version must be 0.95
* The client must have two NICs.
