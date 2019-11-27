//
//  basic.c
//
//
//  Created by 강수진 on 2019/10/29.
//  Modified by Euncho Bae on 2019/11/25.
//
//

//------------------header------------------//
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>

//------------------proc------------------//
#define PROC_DIRNAME "myproc_dir"
#define PROC_FILENAME "myproc_file"
#define BUFF_LEN 4

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_file;

static unsigned short inputPort = 0;

static ssize_t custom_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos) {
    printk(KERN_ALERT "write!\n");
    char my_buffer[BUFF_LEN];
    //유저 프로세스 공간으로 부터 커널로 데이터 복사
    if(copy_from_user(my_buffer, user_buffer, BUFF_LEN)) {
        printk("something happend wrong with copy\n");
    }
    //Convert char array to a specific type - 우리의 경우에는 %u로
    sscanf(my_buffer, "%u", &inputPort);
    if(inputPort == 0) {
        printk(KERN_ALERT "fail to assign inputPort!\n");
        return 0;
    }
    return count;
}

static const struct file_operations custom_proc_fops = {
    .owner = THIS_MODULE,
    .write = custom_write,
};

//------------------chage IP formmat------------------//
int ipStringToNumber (const char* pDottedQuad, unsigned int * pIpAddr)
{
    unsigned int byte3;
    unsigned int byte2;
    unsigned int byte1;
    unsigned int byte0;
    char dummyString[2];
    if (sscanf(pDottedQuad, "%u.%u.%u.%u%1s", &byte3, &byte2, &byte1, &byte0, dummyString) == 4) {
        if ( (byte3 < 256)
            &&(byte2 < 256)
            &&(byte1 < 256)
            &&(byte0 < 256)
            )
        {
            *pIpAddr = (byte3 << 24) + (byte2 << 16) + (byte1 << 8) + byte0;
            return 1;
        }
    }
    return 0;
}
//------------------hooking------------------//
// hook function for PRE_ROUTING hook point
static  unsigned int my_hook_fn_pre(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = tcp_hdr(skb);
    printk("PRE_ROUTING[(%u, %u, %u, %pI4, %pI4)]\n",iph->protocol, ntohs(tcph->source), ntohs(tcph->dest),&iph->saddr, &iph->daddr);
    if (iph->protocol == IPPROTO_TCP)
    {
        if ((tcph->source) == htons(inputPort))
        {
            char *ip = "111.1.1.0";
            unsigned int IpAddr;
            unsigned int changedIpAddr;
            ipStringToNumber(ip, &IpAddr);
            changedIpAddr = htonl(IpAddr);
            tcph->dest = htons(7777);
            iph->daddr = changedIpAddr;
            return NF_ACCEPT; // Tells the system to accept the packet, and process the next one.
        }
    }
    return NF_ACCEPT;
}

// hook function for FORWARD hook point
static  unsigned int my_hook_fn_forward(void *priv, struct sk_buff *skb, const struct nf_hook_state *state){
    struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = tcp_hdr(skb);
    printk("FORWARD[(%u, %u, %u, %pI4, %pI4)]\n",iph->protocol, ntohs(tcph->source), ntohs(tcph->dest),&iph->saddr, &iph->daddr);
    return NF_ACCEPT;
}

// hook function for POST_ROUTING hook point
static  unsigned int my_hook_fn_post(void *priv, struct sk_buff *skb, const struct nf_hook_state *state){
    struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = tcp_hdr(skb);
    printk("POST_ROUTING[(%u, %u, %u, %pI4, %pI4)]\n",iph->protocol, ntohs(tcph->source), ntohs(tcph->dest),&iph->saddr, &iph->daddr);
    return NF_ACCEPT;
}

// hook options for NF_INET_PRE_ROUTING hook point
static struct nf_hook_ops my_nf_ops_pre ={
    .hook = my_hook_fn_pre,
    .pf = PF_INET,
    .hooknum = NF_INET_PRE_ROUTING,
    .priority = 0,
};

// hook options for NF_INET_FORWARD hook point
static struct nf_hook_ops my_nf_ops_forward= {
    .hook = my_hook_fn_forward,
    .pf = PF_INET,
    .hooknum = NF_INET_FORWARD,
    .priority = 0,
};

// hook options for NF_INET_POST_ROUTING hook point
static struct nf_hook_ops my_nf_ops_post ={
    .hook = my_hook_fn_post,
    .pf = PF_INET,
    .hooknum = NF_INET_POST_ROUTING,
    .priority = 0
};

//------------------module setting------------------//
static int __init my_module_init(void) {
    printk(KERN_ALERT "module up...\n");
    //proc
    proc_dir = proc_mkdir(PROC_DIRNAME, NULL);
    proc_file = proc_create(PROC_FILENAME, 0600, proc_dir, &custom_proc_fops);
    // register on hooking point
    nf_register_hook(&my_nf_ops_pre);
    nf_register_hook(&my_nf_ops_forward);
    nf_register_hook(&my_nf_ops_post);
    
    return 0;
}

static void __exit my_module_exit(void) {
    printk(KERN_ALERT "module down...\n");
    //proc
    remove_proc_entry(PROC_FILENAME, proc_dir);
    remove_proc_entry(PROC_DIRNAME, NULL);
    // unregister from hooking point
    nf_unregister_hook(&my_nf_ops_pre);
    nf_unregister_hook(&my_nf_ops_forward);
    nf_unregister_hook(&my_nf_ops_post);
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_AUTHOR("Jin&Cho");
MODULE_DESCRIPTION("System Programming");
MODULE_LICENSE("GPL");
MODULE_VERSION("NEW");

