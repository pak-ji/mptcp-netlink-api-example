#ifndef __USER_EVENT_FUNC_H__
#define __USER_EVENT_FUNC_H__

#include "std.h"
#include "mptcp_nl_api.h"
#include "mptcp_nl_msg.h"

int extract_event(char* res_buff);

struct mp_nl_attr event_created(char* res_buff, bool debug);
struct mp_nl_attr event_established(char* res_buff, bool debug);
struct mp_nl_attr event_announced(char* res_buff, bool debug);
struct mp_nl_attr event_sub_established(char* res_buff, bool debug);
struct mp_nl_attr event_sub_closed(char* res_buff, bool debug);
struct mp_nl_attr event_sub_priority(char* res_buff, bool debug);

#endif /* __USER_EVENT_FUNC_H__ */