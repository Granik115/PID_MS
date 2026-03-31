/*
 * nic.c
 *
 *  Created on: Nov 27, 2024
 *      Author: mvm
 */
#include "net.h"
#include "netif.h"
#include "net_misc.h"

static void notifyLwIPLink(struct netif *netif)
{
  NetInterface *interface = (NetInterface*) netif->state;
  /* Check whether the netif link down and the PHY link is up */
  if (!netif_is_link_up(netif) && (interface->linkState))
  {
    /* network cable is connected */
    netif_set_link_up(netif);
  }
  else if (netif_is_link_up(netif) && (!interface->linkState))
  {
    /* network cable is dis-connected */
    netif_set_link_down(netif);
  }
}

void nicNotifyLinkChange  (NetInterface *interface)
{
  notifyLwIPLink(interface->ipStackInterface);
}

void nicProcessPacket(NetInterface *interface, uint8_t *packet, size_t length,
    NetRxAncillary *ancillary)
{

}

error_t nicSendPacket(NetInterface *interface, const NetBuffer *buffer,
    size_t offset, NetTxAncillary *ancillary)
{
  error_t error = NO_ERROR;
  return error;
}




