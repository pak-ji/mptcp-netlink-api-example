/**
 * @brief	Function to create request messages \
 * 			to be sent to the MPTCP Netlink API
 * 
 * @date	2021-08-22(SUN)
 * @author	Ji-Hun(INSLAB)
 */





#include "../hdr/event_func.h"
#include "../hdr/libnl_api.h"

/**
 * @brief	Create a message to request MPTCP_CMD_ANNOUNCE \
 * 			to the MPTCP Netlink API
 * 
 * @func	create_cmd_announce_req
 * @param	attr 			: Attributes for creating request messages
 * @param	family_id		: ID number of MPTCP Netlink API
 **/
struct nl_msg* create_cmd_announce_req(struct mp_nl_attr attr, int family_id)
{
	struct nl_msg* req;

	uint32_t token = attr.token;
	uint16_t family = attr.family;
	uint8_t loc_id = attr.loc_id;
	uint32_t saddr4 = attr.saddr4;

	req = nlmsg_alloc();

	genlmsg_put(req, getpid(), NL_AUTO_SEQ, family_id, 0, 
						NLM_F_REQUEST, MPTCP_CMD_ANNOUNCE, MPTCP_GENL_VER);

	nla_put(req, MPTCP_ATTR_TOKEN, sizeof(token), token);
	nla_put(req, MPTCP_ATTR_FAMILY, sizeof(family), family);
	nla_put(req, MPTCP_ATTR_LOC_ID, sizeof(loc_id), loc_id);
	nla_put(req, MPTCP_ATTR_SADDR4, sizeof(saddr4), saddr4);

	return req;
}



/**
 * @brief	Create a message to request MPTCP_CMD_SUB_CREATE \
 * 			to the MPTCP Netlink API
 * 
 * @func	create_cmd_sub_create_req
 * @param	attr 			: Attributes for creating request messages
 * @param	family_id		: ID number of MPTCP Netlink API
 **/
struct nl_msg* create_cmd_sub_create_req(struct mp_nl_attr attr, int family_id)
{
	struct nl_msg* req;

	uint32_t token = attr.token;
	uint16_t family = attr.family;
	uint8_t loc_id = attr.loc_id;
	uint8_t rem_id = attr.rem_id;
	uint32_t daddr4 = attr.daddr4;
	uint16_t dport = attr.dport;
	uint8_t backup = attr.backup;

	req = nlmsg_alloc();

	genlmsg_put(req, getpid(), NL_AUTO_SEQ, family_id, 0,
						NLM_F_REQUEST, MPTCP_CMD_SUB_CREATE, MPTCP_GENL_VER);

	nla_put(req, MPTCP_ATTR_TOKEN, sizeof(token), token);
	nla_put(req, MPTCP_ATTR_FAMILY, sizeof(family), family);
	nla_put(req, MPTCP_ATTR_LOC_ID, sizeof(loc_id), loc_id);
	nla_put(req, MPTCP_ATTR_REM_ID, sizeof(rem_id), rem_id);
	nla_put(req, MPTCP_ATTR_DADDR4, sizeof(daddr4), daddr4);
	nla_put(req, MPTCP_ATTR_DPORT, sizeof(dport), dport);
	nla_put(req, MPTCP_ATTR_BACKUP, sizeof(backup), backup);

	return req;
}



/**
 * @brief	Create a message to request MPTECP_CMD_SUB_PRIORITY \
 * 			to the MPTCP Netlink API
 * 
 * @func	create_cmd_sub_priority
 * @param	attr 			: Attributes for creating request messages
 * @param	family_id		: ID number of MPTCP Netlink API
 **/
struct nl_msg* create_cmd_sub_priority_req(struct mp_nl_attr attr, int family_id)
{
	struct nl_msg* req;

	uint32_t token = attr.token;
	uint16_t family = attr.family;
	uint32_t saddr4 = attr.saddr4;
	uint16_t sport = attr.sport;
	uint32_t daddr4 = attr.daddr4;
	uint16_t dport = attr.dport;
	uint8_t backup = attr.backup;

	req = nlmsg_alloc();

	genlmsg_put(req, getpid(), NL_AUTO_SEQ, family_id, 0,
						NLM_F_REQUEST, MPTCP_CMD_SUB_PRIORITY, MPTCP_GENL_VER);

	nla_put(req, MPTCP_ATTR_TOKEN, sizeof(token), token);
	nla_put(req, MPTCP_ATTR_FAMILY, sizeof(family), family);
	nla_put(req, MPTCP_ATTR_SADDR4, sizeof(saddr4), saddr4);
	nla_put(req, MPTCP_ATTR_SPORT, sizeof(sport), sport);
	nla_put(req, MPTCP_ATTR_DADDR4, sizeof(daddr4), daddr4);
	nla_put(req, MPTCP_ATTR_DPORT, sizeof(dport), dport);
	nla_put(req, MPTCP_ATTR_BACKUP, sizeof(backup), backup);

	return req;
}