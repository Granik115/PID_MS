/*
 * net_misc.c
 *
 *  Created on: Feb 28, 2021
 *      Author: maxim
 */

#include "net_misc.h"

//Default options passed to the stack (TX path)
const NetTxAncillary NET_DEFAULT_TX_ANCILLARY =
{ 0,       //Time-to-live value
		false,   //Do not send the packet via a router
#if (IP_DIFF_SERV_SUPPORT == ENABLED)
		0,       //Differentiated services codepoint
#endif
#if (ETH_SUPPORT == ENABLED)
		{
		{
		{ 0 } } }, //Source MAC address
		{
		{
		{ 0 } } }, //Destination MAC address
#endif
#if (ETH_VLAN_SUPPORT == ENABLED)
		-1,      //VLAN priority (802.1Q)
		-1,      //Drop eligible indicator
#endif
#if (ETH_VMAN_SUPPORT == ENABLED)
		-1,      //VMAN priority (802.1ad)
		-1,      //Drop eligible indicator
#endif
#if (ETH_PORT_TAGGING_SUPPORT == ENABLED)
		0,       //Switch port identifier
#endif
#if (ETH_TIMESTAMP_SUPPORT == ENABLED)
		-1,      //Unique identifier for hardware time stamping
#endif
		};

//Default options passed to the stack (RX path)
const NetRxAncillary NET_DEFAULT_RX_ANCILLARY =
{ 0,       //Time-to-live value
#if (ETH_SUPPORT == ENABLED)
		{
		{
		{ 0 } } }, ///<Source MAC address
		{
		{
		{ 0 } } }, ///<Destination MAC address
#endif
#if (ETH_PORT_TAGGING_SUPPORT == ENABLED)
		0,       //Switch port identifier
#endif
#if (ETH_TIMESTAMP_SUPPORT == ENABLED)
		{ 0 },     //Captured time stamp
#endif
		};

