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
	uint32_t token;
	uint16_t family;
	uint32_t saddr4;
	uint32_t daddr4;
	uint16_t sport;
	uint16_t dport;
	uint8_t loc_id;
	uint8_t rem_id;
	uint8_t backup;
};

#endif /*__USER_MPTCP_NL_MSG_H__*/
