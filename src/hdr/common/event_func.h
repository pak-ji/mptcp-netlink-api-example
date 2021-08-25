#ifndef __USER_EVENT_FUNC_H__
#define __USER_EVENT_FUNC_H__

#include "std.h"
#include "mptcp_nl_api.h"
#include "mptcp_nl_msg.h"

struct mp_nl_res extract_event(char* res_buff);

struct mp_nl_attr event_created(struct mp_nl_res res, bool debug);
struct mp_nl_attr event_established(struct mp_nl_res res, bool debug);
struct mp_nl_attr event_closed(struct mp_nl_res res, bool debug);
struct mp_nl_attr event_announced(struct mp_nl_res res, bool debug);
struct mp_nl_attr event_sub_established(struct mp_nl_res res, bool debug);
struct mp_nl_attr event_sub_closed(struct mp_nl_res res, bool debug);
struct mp_nl_attr event_sub_priority(struct mp_nl_res res, bool debug);

#endif /* __USER_EVENT_FUNC_H__ */
