## mptcp-netlink-api-example
* This is an example of the MPTCP Netlink API tested in the environment written below.
* Program Flow
	1. The server and client make an MPTCP initial connection.
	2. The server and client exchange the available IP addresses.
	3. If the MPTCP connection status is Established, the client creates a new subflow.
	4. The client sets the newly connected Subflow to Backup.
	5. When the client enters a "toggle" string, the priority changes between the two subflows.
	6. Repeat v.(5.) until all data is sent.

<br>

## Directory Tree
* src  
ㄴ client : client source  
ㄴ server : server source  
ㄴ func : common function source  
ㄴ hdr : general header source   

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
	- ifstat (for testing)
</pre>

<br>

## Caution
* Must be an MPTCP for multipath-tcp.org.
* The mptcp version must be 0.95 or higher
* The client must have two NICs.
* Servers and clients must not use loopback addresses.

<br>

## Installation Package
<pre>
libnl3 (Must)

$ sudo apt-get install libnl-3-dev libnl-genl-3-dev
</pre>
<pre>
ifstat

$ sudo apt-get install ifstat
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
$ ./src/mptcp_nl_server
</pre>
##### in client
<pre>
$ ./src/mptcp_nl_client
</pre>

<br>

## Video
* Preparing...
