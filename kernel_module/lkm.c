#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tobias Gierke");
MODULE_DESCRIPTION("Looks for special UDP broadcast packets and triggers a PPS event for them.");
MODULE_VERSION("0.01");

#define EXPECTED_PACKET_LEN 32
#define DATA_OFFSET EXPECTED_PACKET_LEN - 4

static struct nf_hook_ops *nfho = NULL;

static unsigned int lkm_hfunc(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
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
          printk(KERN_INFO "Received PPS packet\n");
          return NF_DROP;
        }
        printk(KERN_INFO "No magic payload\n");
      } else {
        printk(KERN_INFO "Bad packet data length %d\n",skb->data_len);
        printk(KERN_INFO "Bad packet length %d\n",skb->len);
      }
    } else {
      printk(KERN_INFO "Destination is not a broadcast address %d\n",iph->daddr);
    }
  }
  return NF_ACCEPT;
}

static int __init lkm_init(void)
{
  nfho = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);

  nfho->hook 	= (nf_hookfn*) lkm_hfunc;
  nfho->hooknum 	= NF_INET_PRE_ROUTING;
  nfho->pf 	= PF_INET;
  nfho->priority 	= NF_IP_PRI_FIRST;

  nf_register_net_hook(&init_net, nfho);
  printk(KERN_INFO "PPS packet filter hook registered\n");
  return 0;
}

static void __exit lkm_exit(void)
{
  printk(KERN_INFO "PPS packet filter hook unloaded\n");
  nf_unregister_net_hook(&init_net, nfho);
  kfree(nfho);
}

module_init(lkm_init);
module_exit(lkm_exit);
