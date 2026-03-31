/*
 * net_misc.h
 *
 *  Created on: Feb 27, 2021
 *      Author: maxim
 */

#ifndef INC_NET_MISC_H_
#define INC_NET_MISC_H_

#include "stdbool.h"
#include "net.h"

typedef struct _NetTxAncillary NetTxAncillary;
typedef struct _NetRxAncillary NetRxAncillary;
/**
 * @brief Additional options passed to the stack (TX path)
 **/

struct _NetTxAncillary
{
	uint8_t ttl;         ///<Time-to-live value
	bool dontRoute;    ///<Do not send the packet via a router
#if (IP_DIFF_SERV_SUPPORT == ENABLED)
	uint8_t dscp;        ///<Differentiated services codepoint
#endif
#if (ETH_SUPPORT == ENABLED)
	MacAddr srcMacAddr;  ///<Source MAC address
	MacAddr destMacAddr; ///<Destination MAC address
#endif
#if (ETH_VLAN_SUPPORT == ENABLED)
	int8_t vlanPcp;      ///<VLAN priority (802.1Q)
	int8_t vlanDei;      ///<Drop eligible indicator
#endif
#if (ETH_VMAN_SUPPORT == ENABLED)
	int8_t vmanPcp;      ///<VMAN priority (802.1ad)
	int8_t vmanDei;      ///<Drop eligible indicator
#endif
#if (ETH_PORT_TAGGING_SUPPORT == ENABLED)
	uint8_t port;        ///<Switch port identifier
#endif
#if (ETH_TIMESTAMP_SUPPORT == ENABLED)
	int32_t timestampId; ///<Unique identifier for hardware time stamping
#endif
};

/**
 * @brief Timestamp
 **/

typedef struct
{
	uint32_t s;
	uint32_t ns;
} NetTimestamp;

/**
 * @brief Additional options passed to the stack (RX path)
 **/

struct _NetRxAncillary
{
	uint8_t ttl;            ///<Time-to-live value
#if (ETH_SUPPORT == ENABLED)
	MacAddr srcMacAddr;     ///<Source MAC address
	MacAddr destMacAddr;    ///<Destination MAC address
#endif
#if (ETH_PORT_TAGGING_SUPPORT == ENABLED)
	uint8_t port;           ///<Switch port identifier
#endif
#if (ETH_TIMESTAMP_SUPPORT == ENABLED)
	NetTimestamp timestamp; ///<Captured time stamp
#endif
};

//Global constants
extern const NetTxAncillary NET_DEFAULT_TX_ANCILLARY;
extern const NetRxAncillary NET_DEFAULT_RX_ANCILLARY;

#endif /* INC_NET_MISC_H_ */
