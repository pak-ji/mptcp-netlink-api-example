#include "../hdr/client/lookup_nic.h"

char** lookup_nic(int* ret_iface_num)
{
	struct ifaddrs *ifaces, *iface;
	int iface_num = 0;

	char *nic_addr;
	char *nic_name;
	char **nic;

	getifaddrs(&ifaces);
	iface = ifaces;

	while(iface)
	{
		if(iface->ifa_addr && iface->ifa_addr->sa_family == AF_INET){
			nic_name = iface->ifa_name;
			nic_addr = inet_ntoa(((struct sockaddr_in*)iface->ifa_addr)->sin_addr);

			if(strcmp(nic_name, "lo")){
				iface_num++;

				if(iface_num == 1)
					nic = (char**)malloc(iface_num*sizeof(char*));
				else
					nic = (char**)realloc(nic, iface_num*sizeof(char*));

				nic[iface_num-1] = (char*)malloc((strlen(nic_name)+strlen(nic_addr)+1)*sizeof(char));

				strcpy(nic[iface_num-1], nic_name);
				strcpy(&nic[iface_num-1][strlen(nic_name)], ":");
				strcpy(&nic[iface_num-1][strlen(nic_name)+1], nic_addr);
			}
		}
		iface = iface->ifa_next;
	}
	freeifaddrs(ifaces);

	*ret_iface_num = iface_num;
	return nic;
}
