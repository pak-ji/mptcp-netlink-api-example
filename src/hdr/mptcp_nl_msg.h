#ifndef __USER_MPTCP_NL_MSG_H__
#define __USER_MPTCP_NL_MSG_H__

#include "std.h"
#include "libnl_api.h"

struct mp_nl_req{
	struct nl_msg* req;
};

struct mp_nl_res{
	struct nlmsghdr* nlh;
	struct genlmsghdr* genlh;
	struct nlattr* nla;
	int msg_len;
	int payload_len;
};

struct mp_nl_err{
	struct nlmsghdr* nlh;
	struct nlmsgerr* nlerr;
};

struct mp_nl_attr{
	uint32_t token = 0;
	uint16_t family = 0;
	uint32_t saddr4 = 0;
	uint32_t daddr4 = 0;
	uint16_t sport = 0;
	uint16_t dport = 0;
	uint8_t loc_id = 0;
	uint8_t rem_id = 0;
	uint8_t backup = 0;
};

#endif /*__USER_MPTCP_NL_MSG_H__*/