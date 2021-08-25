/**
 * @brief	Function to extract contents of \
 * 			event messages received from \
 *			MPTCP Generic Netlink API
 * 
 * @date	2021-08-22(SUN)
 * @author	Ji-Hun(INSLAB)
 */






#include "../hdr/common/event_func.h"
#include "../hdr/common/libnl_api.h"

/**
 * @brief	extract event from response message
 * 
 * @func	extract_event
 * @param	res_buff		: Received event message buff
 **/
struct mp_nl_res extract_event(char* res_buff)
{
	struct mp_nl_res res;

	res.nlh = (struct nlmsghdr*)res_buff;
	res.genlh = (struct genlmsghdr*)genlmsg_hdr(res.nlh);
	res.nla = (struct nlattr*)genlmsg_data(res.genlh);
	res.msg_len = res.nlh->nlmsg_len;
	res.payload_len = res.msg_len - (16 + 4); // 16 = NLMSG_HDRLEN / 4 = GENLMSG_HDRLEN
	
	return res;
}



/**
 * @brief	Extract netlink attributes from response message(EVENT_CREATED)
 * 
 * @func	event_created
 * @param	res				: Received event message
 * @param	debug			: Determine debug output
 **/
struct mp_nl_attr event_created(struct mp_nl_res res, bool debug)
{
	struct mp_nl_attr attr;

	memset(&attr, 0, sizeof(struct mp_nl_attr));
	if(res.genlh->cmd != MPTCP_EVENT_CREATED){
		fprintf(stderr, "ERROR) event is not MPTCP_EVENT_CREATED");
		return attr; /* structure field all zero */
	}

	attr.token = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_TOKEN));
	attr.family = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_FAMILY));
	attr.saddr4 = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_SADDR4));
	attr.daddr4 = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_DADDR4));
	attr.sport = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_SPORT));
	attr.dport = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_DPORT));

	if(debug){
		uint8_t* ptr;

		printf("INFO) Received event is MPTCP_EVENT_CREATED\n");
		printf("\tTOKEN) %04X\n", attr.token);
		
		ptr = (uint8_t*)&attr.saddr4;
		printf("\tSOURCE) %u.%u.%u.%u:%u\n", *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), attr.sport);

		ptr = (uint8_t*)&attr.daddr4;
		printf("\tDESTINATION) %u.%u.%u.%u:%u\n", *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), attr.dport);
	}

	return attr;
}



/**
 * @brief	Extract netlink attributes from response message(EVENT_ESTABLISHED)
 * 
 * @func	event_established
 * @param	res				: Received event message
 * @param	debug			: Determine debug output
 **/
struct mp_nl_attr event_established(struct mp_nl_res res, bool debug)
{
	struct mp_nl_attr attr;

	memset(&attr, 0, sizeof(struct mp_nl_attr));
	if(res.genlh->cmd != MPTCP_EVENT_ESTABLISHED){
		fprintf(stderr, "ERROR) event is not MPTCP_EVENT_ESTABLISHED");
		return attr; /* structure field all zero */
	}

	attr.token = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_TOKEN));
	attr.family = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_FAMILY));
	attr.saddr4 = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_SADDR4));
	attr.daddr4 = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_DADDR4));
	attr.sport = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_SPORT));
	attr.dport = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_DPORT));

	if(debug){
		uint8_t* ptr;

		printf("INFO) Received event is MPTCP_EVENT_ESTABLISEHD\n");
		printf("\tTOKEN) %04X\n", attr.token);
		
		ptr = (uint8_t*)&attr.saddr4;
		printf("\tSOURCE) %u.%u.%u.%u:%u\n", *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), attr.sport);

		ptr = (uint8_t*)&attr.daddr4;
		printf("\tDESTINATION) %u.%u.%u.%u:%u\n", *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), attr.dport);
	}

	return attr;
}



/**
 * @brief	Extract netlink attributes from response message(EVENT_CLOSED)
 * 
 * @func	event_closed
 * @param	res				: Received event message
 * @param	debug			: Determine debug output
 **/
struct mp_nl_attr event_closed(struct mp_nl_res res, bool debug)
{	
	struct mp_nl_attr attr;

	memset(&attr, 0, sizeof(struct mp_nl_attr));
	if(res.genlh->cmd != MPTCP_EVENT_CLOSED){
		fprintf(stderr, "ERROR) event is not MPTCP_EVENT_CLOSED");
		return attr; /* structure field all zero */
	}

	attr.token = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_TOKEN));

	if(debug){
		printf("INFO) Received event is MPTCP_EVENT_CLOSED\n");
		printf("\tTOKEN) %04X\n", attr.token);
	}

	return attr;
}


/**
 * @brief	Extract netlink attributes from response message(EVENT_ANNOUNCED)
 * 
 * @func	event_announced
 * @param	res				: Received event message
 * @param	debug			: Determine debug output
 **/
struct mp_nl_attr event_announced(struct mp_nl_res res, bool debug)
{
	struct mp_nl_attr attr;

	memset(&attr, 0, sizeof(struct mp_nl_attr));
	if(res.genlh->cmd != MPTCP_EVENT_ANNOUNCED){
		fprintf(stderr, "ERROR) event is not MPTCP_EVENT_ANNOUNCED");
		return attr; /* structure field all zero */
	}

	attr.token = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_TOKEN));
	attr.rem_id = *(uint8_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_REM_ID));
	attr.family = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_FAMILY));
	attr.daddr4 = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_DADDR4));

	if(debug){
		uint8_t* ptr;

		printf("INFO) Received event is MPTCP_EVENT_ANNOUNCED\n");
		printf("\tTOKEN) %04X\n", attr.token);
	
		ptr = (uint8_t*)&attr.daddr4;
		printf("\tDADDR4) %u.%u.%u.%u\n", *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3));

		printf("\tREM_ID) %u\n", attr.rem_id);
	}

	return attr;
}



/**
 * @brief	Extract netlink attributes from response message(EVENT_SUB_ESTABLISHED)
 * 
 * @func	event_sub_established
 * @param	res				: Received event message
 * @param	debug			: Determine debug output
 **/
struct mp_nl_attr event_sub_established(struct mp_nl_res res, bool debug)
{
	struct mp_nl_attr attr;

	memset(&attr, 0, sizeof(struct mp_nl_attr));
	if(res.genlh->cmd != MPTCP_EVENT_SUB_ESTABLISHED){
		fprintf(stderr, "ERROR) event is not MPTCP_EVENT_SUB_ESTABLISHED");
		return attr; /* structure field all zero */
	}

	attr.token = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_TOKEN));
	attr.family = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_FAMILY));
	attr.saddr4 = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_SADDR4));
	attr.daddr4 = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_DADDR4));
	attr.sport = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_SPORT));
	attr.dport = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_DPORT));
	attr.backup = *(uint8_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_BACKUP));

	if(debug){
		uint8_t* ptr;

		printf("INFO) Received event is MPTCP_EVENT_SUB_ESTABLISHED\n");
		printf("\tTOKEN) %04X\n", attr.token);
		
		ptr = (uint8_t*)&attr.saddr4;
		printf("\tSOURCE) %u.%u.%u.%u:%u\n", *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), attr.sport);

		ptr = (uint8_t*)&attr.daddr4;
		printf("\tDESTINATION) %u.%u.%u.%u:%u\n", *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), attr.dport);

		printf("\tPRIORITY) %s\n", (attr.backup?"Backup":"Primary"));
	}

	return attr;
}



/**
 * @brief	Extract netlink attributes from response message(EVENT_SUB_CLOSED)
 * 
 * @func	event_sub_closed
 * @param	res				: Received event message
 * @param	debug			: Determine debug output
 **/
struct mp_nl_attr event_sub_closed(struct mp_nl_res res, bool debug)
{
	struct mp_nl_attr attr;

	memset(&attr, 0, sizeof(struct mp_nl_attr));
	if(res.genlh->cmd != MPTCP_EVENT_SUB_CLOSED){
		fprintf(stderr, "ERROR) event is not MPTCP_EVENT_SUB_CLOSED");
		return attr; /* structure field all zero */
	}

	attr.token = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_TOKEN));
	attr.family = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_FAMILY));
	attr.saddr4 = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_SADDR4));
	attr.daddr4 = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_DADDR4));
	attr.sport = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_SPORT));
	attr.dport = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_DPORT));
	attr.backup = *(uint8_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_BACKUP));

	if(debug){
		uint8_t* ptr;

		printf("INFO) Received event is MPTCP_EVENT_SUB_CLOSED\n");
		printf("\tTOKEN) %04X\n", attr.token);
		
		ptr = (uint8_t*)&attr.saddr4;
		printf("\tSOURCE) %u.%u.%u.%u:%u\n", *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), attr.sport);

		ptr = (uint8_t*)&attr.daddr4;
		printf("\tDESTINATION) %u.%u.%u.%u:%u\n", *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), attr.dport);

		printf("\tPRIORITY) %s\n", (attr.backup?"Backup":"Primary"));
	}

	return attr;
}



/**
 * @brief	Extract netlink attributes from response message(EVENT_SUB_PRIORITY)
 * 
 * @func	event_sub_priority
 * @param	res				: Received event message
 * @param	debug			: Determine debug output
 **/
struct mp_nl_attr event_sub_priority(struct mp_nl_res res, bool debug)
{
	struct mp_nl_attr attr;

	memset(&attr, 0, sizeof(struct mp_nl_attr));
	if(res.genlh->cmd != MPTCP_EVENT_SUB_PRIORITY){
		fprintf(stderr, "ERROR) event is not MPTCP_EVENT_SUB_PRIORITY");
		return attr; /* structure field all zero */
	}

	attr.token = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_TOKEN));
	attr.family = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_FAMILY));
	attr.saddr4 = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_SADDR4));
	attr.daddr4 = *(uint32_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_DADDR4));
	attr.sport = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_SPORT));
	attr.dport = *(uint16_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_DPORT));
	attr.backup = *(uint8_t*)nla_data(nla_find(res.nla, res.payload_len, MPTCP_ATTR_BACKUP));

	if(debug){
		uint8_t* ptr;

		printf("INFO) Received event is MPTCP_EVENT_SUB_PRIORITY\n");
		printf("\tTOKEN) %04X\n", attr.token);
		
		ptr = (uint8_t*)&attr.saddr4;
		printf("\tSOURCE) %u.%u.%u.%u:%u\n", *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), attr.sport);

		ptr = (uint8_t*)&attr.daddr4;
		printf("\tDESTINATION) %u.%u.%u.%u:%u\n", *(ptr+0), *(ptr+1), *(ptr+2), *(ptr+3), attr.dport);

		printf("\tPRIORITY) %s\n", (attr.backup?"Backup":"Primary"));
	}

	return attr;
}
