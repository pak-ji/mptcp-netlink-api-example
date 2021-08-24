#ifndef __USER_LOOKUP_NIC_H__
#define __USER_LOOKUP_NIC_H__

#include <sys/types.h>
#include <ifaddrs.h>
#include <string.h>
#include <arpa/inet.h>

#include "../common/std.h"

char** lookup_nic();

#endif /* __USER_LOOKUP_NIC_H__ */
