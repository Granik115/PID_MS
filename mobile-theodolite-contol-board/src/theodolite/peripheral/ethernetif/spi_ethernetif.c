/**
 ******************************************************************************
 * File Name          : ethernetif.c
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip/opt.h"

#include "lwip/timeouts.h"
#include "netif/ethernet.h"
#include "netif/etharp.h"
#include "lwip/ethip6.h"
#include <string.h>
#include <theodolite/peripheral/ethernetif/spi_ethernetif.h>
//#include "cmsis_os.h"
#include "lwip/tcpip.h"
#include "enc28j60_driver.h"

/* Private define ------------------------------------------------------------*/
/* The time to block waiting for input. */
#define TIME_WAITING_FOR_INPUT ( portMAX_DELAY )
/* USER CODE BEGIN OS_THREAD_STACK_SIZE_WITH_RTOS */
/* USER CODE END OS_THREAD_STACK_SIZE_WITH_RTOS */
#define ETH_MAX_PACKET_SIZE       1524U    /*!< ETH_HEADER + ETH_EXTRA + ETH_VLAN_TAG + ETH_MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_TX_BUF_SIZE ETH_MAX_PACKET_SIZE
#define ETH_RX_BUF_SIZE ETH_MAX_PACKET_SIZE
extern const SpiDriver spi1driver;
extern const ExtIntDriver ext1driver;

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
  /* USER CODE BEGIN OS_THREAD_ATTR_CMSIS_RTOS_V2 */
  NetInterface *interface = (NetInterface*)netif->state;
  /* USER CODE END OS_THREAD_ATTR_CMSIS_RTOS_V2 */
  /* Init ETH */
  interface->macAddr.b[0] = 0x00;
  interface->macAddr.b[1] = 0x80;
  interface->macAddr.b[2] = 0xE2;
  interface->macAddr.b[3] = 0x00;
  interface->macAddr.b[4] = 0x00;
  interface->macAddr.b[5] = 0x00;

#ifdef MKTI_OS
  osMutexAcquire(interface->mutex, osWaitForever);
#endif
  error_t init_status = enc28j60Init(interface);
#ifdef MKTI_OS
  osMutexRelease(interface->mutex);
#endif

  if (init_status == NO_ERROR)
  {
    /* Set netif link flag */
    netif->flags |= NETIF_FLAG_LINK_UP;

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;
    /* set MAC hardware address */
    netif->hwaddr[0] = interface->macAddr.b[0];
    netif->hwaddr[1] = interface->macAddr.b[1];
    netif->hwaddr[2] = interface->macAddr.b[2];
    netif->hwaddr[3] = interface->macAddr.b[3];
    netif->hwaddr[4] = interface->macAddr.b[4];
    netif->hwaddr[5] = interface->macAddr.b[5];
    /* maximum transfer unit */
    netif->mtu = 1500;
    /* Accept broadcast address and ARP traffic */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
  }
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  NetInterface *interface = (NetInterface*)netif->state;
  pbuf_ref(p);
#ifdef MKTI_OS
  const osStatus_t status = osMessageQueuePut(interface->tx_message_queue,
                                              (void*)&p,
                                              0, osWaitForever);
  if (status != osOK)
    pbuf_free(p);
#else
  const err_t status = 0;
#endif

  return (err_t)status;
}

static uint32_t EthernetLinkTimer = 0;

void spi_ethernetif_set_link(struct netif *netif)
{
  uint32_t regvalue = 0;
  /* Ethernet Link every 200ms */
  if (HAL_GetTick() - EthernetLinkTimer >= 200)
  {
    EthernetLinkTimer = HAL_GetTick();

    /* Read PHY_BSR*/
    //HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &regvalue);
    regvalue &= PHY_LINKED_STATUS;

    /* Check whether the netif link down and the PHY link is up */
    if (!netif_is_link_up(netif) && (regvalue))
    {
      /* network cable is connected */
      netif_set_link_up(netif);
    }
    else if (netif_is_link_up(netif) && (!regvalue))
    {
      /* network cable is disconnected */
      netif_set_link_down(netif);
    }
  }
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void spi_ethernetif_input(struct netif *netif)
{
  err_t err;
  struct pbuf *p;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);

  /* no packet could be read, silently ignore this */
  if (p == NULL)
    return;

  /* entry point to the LwIP stack */
  err = netif->input(p, netif);

  if (err != ERR_OK)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
    p = NULL;
  }
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t spi_ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  netif->name[0] = ((NetInterface*)netif->state)->name[0];
  netif->name[1] = ((NetInterface*)netif->state)->name[1];
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
  netif->output = etharp_output;
#else
  /* The user should write ist own code in low_level_output_arp_off function */
  netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

  netif->linkoutput = low_level_output;

  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

void ethernetif_update_config(struct netif *netif)
{

}

