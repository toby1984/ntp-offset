#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

// network includes
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

// PPS includes
#include <linux/time.h>
#include <linux/errno.h>
#include <linux/pps_kernel.h>

/*
 * A Linux kernel module that registers a PPS source
 * driven by UDP broadcast packets containing
 * only the string 'time' as payload.
 *
 * Whenever such a UDP packet is received, a PPS capture
 * event will be triggered.
 * 
 * (C) 2019 tobias.gierke@code-sourcery.de
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tobias.gierke@code-sourcery.de");
MODULE_DESCRIPTION("Looks for UDP broadcast packets with the string 'time' as payload and triggers a PPS event whenever one is received.");
MODULE_VERSION("0.01");
MODULE_SOFTDEP("pre: pps_core");

// #define DEBUG
#define EXPECTED_PACKET_LEN 32
#define DATA_OFFSET EXPECTED_PACKET_LEN - 4

static struct pps_device *pps = NULL;
static struct nf_hook_ops *nfho = NULL;


static struct pps_source_info pps_ktimer_info = {
  .name		= "crudepps",
  .path		= "",
  .mode		= PPS_CAPTUREASSERT | PPS_OFFSETASSERT |  PPS_ECHOASSERT |  PPS_CANWAIT | PPS_TSFMT_TSPEC,
  .owner		= THIS_MODULE,
};

static unsigned int crudepps_hfunc(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
  struct pps_event_time ts;
  struct iphdr *iph;
  if (!skb) {
    return NF_ACCEPT;
  }

  iph = ip_hdr(skb);
  // look for UDP broadcast packet with our magic payload
  if (iph->protocol == IPPROTO_UDP ) {
    if ( iph->daddr == 0xffffffff ) {
      if ( skb->len == EXPECTED_PACKET_LEN)
      {
        if (
          skb->data[DATA_OFFSET+0] == 't' &&
          skb->data[DATA_OFFSET+1] == 'i' &&
          skb->data[DATA_OFFSET+2] == 'm' &&
          skb->data[DATA_OFFSET+3] == 'e')
        {
#ifdef DEBUG
          printk(KERN_INFO "Received PPS packet\n");
#endif

          // trigger PPS event
          pps_get_ts(&ts);
          pps_event(pps, &ts, PPS_CAPTUREASSERT, NULL);

          return NF_DROP;
        }
#ifdef DEBUG
        printk(KERN_INFO "No magic payload\n");
#endif
      } else {
#ifdef DEBUG
        printk(KERN_INFO "Bad packet data length %d\n",skb->data_len);
        printk(KERN_INFO "Bad packet length %d\n",skb->len);
#endif
      }
    } else {
#ifdef DEBUG
      printk(KERN_INFO "Destination is not a broadcast address %d\n",iph->daddr);
#endif
    }
  }
  return NF_ACCEPT;
}

static int __init crudepps_init(void)
{
  // register netfilter hook
  printk(KERN_INFO "Registering netfilter hook\n");
  nfho = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);
  if ( IS_ERR(nfho) ) {
    pr_err("Failed to allocate memory\n");
    return -ENOMEM;
  }
  nfho->hook 	= (nf_hookfn*) crudepps_hfunc;
  nfho->hooknum 	= NF_INET_PRE_ROUTING;
  nfho->pf 	= PF_INET;
  nfho->priority 	= NF_IP_PRI_FIRST;

  if ( nf_register_net_hook(&init_net, nfho) ) {
    pr_err("Failed to register netfilter hook\n");
    return -EINVAL;
  }

  // register PPS
  printk(KERN_INFO "Registering PPS source\n");
  pps = pps_register_source(&pps_ktimer_info,PPS_CAPTUREASSERT | PPS_OFFSETASSERT);
  if (IS_ERR(pps)) {
    pr_err("Failed to register PPS source\n");
    return PTR_ERR(pps);
  }
  return 0;
}

static void __exit crudepps_exit(void)
{
  // unregister netfilter hook
  if ( nfho )
  {
    printk(KERN_INFO "Unregistering netfilter hook\n");
    nf_unregister_net_hook(&init_net, nfho);
    kfree(nfho);
  }

  // unregister PPS source
  if ( pps ) {
    printk(KERN_INFO "Unregistering PPS source\n");
    pps_unregister_source(pps);
  }
}

module_init(crudepps_init);
module_exit(crudepps_exit);
