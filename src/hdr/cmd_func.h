#ifndef __USER_CMD_FUNC_H__
#define __USER_CMD_FUNC_H__

#include "mptcp_nl_api.h"
#include "mptcp_nl_msg.h"

struct nl_msg* create_cmd_announce_req(struct mp_nl_attr attr);
struct nl_msg* create_cmd_sub_create_req(struct mp_nl_attr attr);
struct nl_msg* create_cmd_sub_priority_req(struct mp_nl_attr attr);

#endif /* __USER_CMD_FUNC_H__ */