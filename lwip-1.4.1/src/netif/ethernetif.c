/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"

#if 1 /* don't build, this is only a skeleton, see previous comment */

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"

#include <common.h>
#include "ethernetif.h"
#include <net.h>
#include <asm/addrspace.h>

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

uchar		NetOurEther[6] = {0};		/* Our ethernet address			*/
volatile uchar *NetRxPackets[PKTBUFSRX] = {NULL}; /* Receive packets			*/
extern volatile uchar  *NetTxPacket; // volatile uchar *NetTxPacket = 0;	/* THE transmit packet */

/**********************************************************************/

IPaddr_t	NetArpWaitPacketIP = {0};
IPaddr_t	NetArpWaitReplyIP = {0};

uchar	    *NetArpWaitPacketMAC;	/* MAC address of waiting packet's destination	*/
uchar       *NetArpWaitTxPacket;	/* THE transmit packet			*/
int			NetArpWaitTxPacketSize;
uchar 		NetArpWaitPacketBuf[PKTSIZE_ALIGN + PKTALIGN];
ulong		NetArpWaitTimerStart;

extern volatile uchar * NetTxPacket;		/* THE transmit packet		*/
extern volatile uchar * NetRxPkt;		/* Current receive packet	*/
extern int		NetRxPktLen;		/* Current rx packet length	*/

int			NetArpWaitTry;

//===================================================
/*=======================================*/
extern VALID_BUFFER_STRUCT  rt2880_free_buf_list;
//kaiker
extern BUFFER_ELEM *rt2880_free_buf_entry_dequeue(VALID_BUFFER_STRUCT *hdr);
//定义发送接受缓冲区
u8 lwip_buf[1500*2] = {0};
// max frame length which the conroller will accept:
#define   MAX_FRAMELEN    1518
// (note: maximum ethernet frame length would be 1518)


/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/* Forward declarations. */
err_t ethernetif_input(struct netif *netif); //static void  ethernetif_input(struct netif *netif);


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */

extern IPaddr_t   tmp_ip_addr;

void init_net_device(void)
{
	DECLARE_GLOBAL_DATA_PTR; // 注意: 只能 定义一次
	bd_t *bd = gd->bd;
	unsigned char ethinit_attempt = 0;

#ifdef CONFIG_NET_MULTI
	  NetRestarted = 0;
	  NetDevExists = 0;
#endif
	  /* XXX problem with bss workaround */
	  NetArpWaitPacketMAC = NULL;
	  NetArpWaitTxPacket = NULL;
	  NetArpWaitPacketIP = 0;
	  NetArpWaitReplyIP = 0;
	  NetArpWaitTxPacket = NULL;
#ifdef DEBUG	
	 printf("File: %s, Func: %s, Line: %d\n", __FILE__,__FUNCTION__ , __LINE__);
#endif   

	  if (!NetTxPacket) {
		  int i;
		  BUFFER_ELEM *buf;
		  /*
		   *  Setup packet buffers, aligned correctly.
		   */
		  buf = rt2880_free_buf_entry_dequeue(&rt2880_free_buf_list); 
		  NetTxPacket = buf->pbuf;

		  debug("\n NetTxPacket = 0x%08X \n",NetTxPacket);
		  for (i = 0; i < NUM_RX_DESC; i++) {
  
			  buf = rt2880_free_buf_entry_dequeue(&rt2880_free_buf_list); 
			  if(buf == NULL)
			  {
				  printf("\n Packet Buffer is empty ! \n");
				  return (-1);
			  }
			  NetRxPackets[i] = buf->pbuf;
			  //printf("\n NetRxPackets[%d] = 0x%08X\n",i,NetRxPackets[i]);
		  }
	  }

	NetTxPacket = KSEG1ADDR(NetTxPacket);
  
	  printf("\n KSEG1ADDR(NetTxPacket) = 0x%08X \n",NetTxPacket);
  
	  if (!NetArpWaitTxPacket) {
		  NetArpWaitTxPacket = &NetArpWaitPacketBuf[0] + (PKTALIGN - 1);
		  NetArpWaitTxPacket -= (ulong)NetArpWaitTxPacket % PKTALIGN;
		  NetArpWaitTxPacketSize = 0;
	}

restart:		  
	  printf("\n NetLoop,call eth_halt ! \n");
	  eth_halt();
#ifdef CONFIG_NET_MULTI
	  eth_set_current();
#endif
	  while( ethinit_attempt < 10 ) {
	  	  if ( eth_init( bd ) ) {
		  	  ethinit_attempt = 0;
		  	  break;
		    } else {
		  	  ethinit_attempt++;
		  	  eth_halt();
		  	  udelay( 1000000 );
		    }
	  }
  
	  if ( ethinit_attempt > 0 ) {
		  eth_halt();
		  printf( "## Error: couldn't initialize eth (cable disconnected?)!\n\n" );
		  return( -1 );
	  }

#ifdef CONFIG_NET_MULTI
	  memcpy (NetOurEther, eth_get_dev()->enetaddr, 6);
#else
	  memcpy (NetOurEther, bd->bi_enetaddr, 6);
#endif
}

static void
low_level_init(struct netif *netif)
{
  struct ethernetif *ethernetif = netif->state;
  
  init_net_device(); // 初始化网卡, do  in  NetLoopHttpd();

  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] =NetOurEther[0];
  netif->hwaddr[1] =NetOurEther[1];
  netif->hwaddr[2] =NetOurEther[2];
  netif->hwaddr[3] =NetOurEther[3];
  netif->hwaddr[4] =NetOurEther[4];
  netif->hwaddr[5] =NetOurEther[5];
  printf("MAC  %02X:%02X:%02X:%02X:%02X:%02X\n", netif->hwaddr[0], netif->hwaddr[1],
  	netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

  /* maximum transfer unit */
  netif->mtu = 1500;//最大允许传输单元，允许该网卡广播和ARP功能
  
  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
 
  /* Do whatever else is needed to initialize interface. */  
  // [TODO]
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct ethernetif *ethernetif = netif->state;
  struct pbuf *q = {0};
  unsigned int i = 0; 

  // [TODO] initiate transfer();  初始化 网卡的Tx函数
  
#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

#if 0
  for(q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */
    // [TODO]  send data from(q->payload, q->len); 网卡 发送数据
    {
      memcpy(&NetTxPacket[i], (u8_t*)q->payload, q->len); 
      i = i + q->len;
      eth_send(NetTxPacket, i);
    }
  }
#else
      pbuf_copy_partial(p, /* mac_send_buffer */ NetTxPacket, p->tot_len, 0);
      eth_send(NetTxPacket, p->tot_len);
#endif

  //[TODO] signal that packet should be sent();

#if ETH_PAD_SIZE
  pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
  
  LINK_STATS_INC(link.xmit);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct ethernetif *ethernetif = netif->state;
  struct pbuf *p = {NULL}, *q = {NULL};
  u16_t len;
  unsigned int i =0;

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  // [TODO] len = ;
  len =  eth_rx();

#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

  /* We allocate a pbuf chain of pbufs from the pool. */
if(NetRxPkt != NULL)
  p = pbuf_alloc(PBUF_RAW, NetRxPktLen, PBUF_POOL);

  if (p != NULL) {

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

#if 0
    /* We iterate over the pbuf chain until we have read the entire
     * packet into the pbuf. */
    for(q = p; q != NULL; q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
       * available data in the pbuf is given by the q->len
       * variable.
       * This does not necessarily have to be a memcpy, you can also preallocate
       * pbufs for a DMA-enabled MAC and after receiving truncate it to the
       * actually received size. In this case, ensure the tot_len member of the
       * pbuf is the sum of the chained pbuf len members.
       */
      //[TODO] read data into(q->payload, q->len);
      memcpy((u8_t*)q->payload, (u8_t*)&NetRxPkt[i], q->len);
      i = i + q->len;
    }
    //[TODO] acknowledge that packet has been read();
#else
      pbuf_take(p, NetRxPkt,  NetRxPktLen); // [NetRxPkt : Rx数据包指针];   [NetRxPktLen : Rx数据包的长度]
#endif


#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.recv);
  } else {
    //[TODO] drop packet();
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
  }

  return p;  
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
err_t // static void
ethernetif_input(struct netif *netif)
{
  struct ethernetif *ethernetif = NULL;
  struct eth_hdr *ethhdr = NULL;
  struct pbuf *p = NULL;

  ethernetif = netif->state;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);
  /* no packet could be read, silently ignore this */
  if (p == NULL)
  {
  	printf("[ERR]_p=NULL; low_level_input()\n");
  	return;
  }
  /* points to packet payload, which starts with an Ethernet header */
  ethhdr = p->payload;

  switch (htons(ethhdr->type)) {
  /* IP or ARP packet? */
  case ETHTYPE_IP:
  case ETHTYPE_ARP:
#if PPPOE_SUPPORT
  /* PPPoE packet? */
  case ETHTYPE_PPPOEDISC:
  case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
    /* full packet send to tcpip_thread to process */
    if (netif->input(p, netif)!=ERR_OK)
     { LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
       pbuf_free(p);
       p = NULL;
     }
    break;

  default:
    pbuf_free(p);
    p = NULL;
    break;
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
err_t
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));
    
  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  
  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  
  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}

#endif /* 0 */
