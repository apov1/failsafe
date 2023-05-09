#include "Netlink/Connection.h"

#include <linux/ktime.h>
#include <linux/wait.h>
#include <net/genetlink.h>

#include "Common/EventType.h"
#include "Common/NetlinkFamily.h"

#include "Hooking/ResponseWaitMap.h"

int fshNLRegisterDoit( struct sk_buff *senderSkb, struct genl_info *info );
int fshNLReplyDoit( struct sk_buff *senderSkb, struct genl_info *info );

struct genl_ops __fshNLOps[] = { {
                                     .cmd = FSH_NL_C_REGISTER,
                                     .doit = fshNLRegisterDoit,
                                     .flags = GENL_ADMIN_PERM,
                                 },
                                 {
                                     .cmd = FSH_NL_C_REPLY,
                                     .doit = fshNLReplyDoit,
                                     .flags = GENL_ADMIN_PERM,
                                 } };

static struct nla_policy __fshNLPolicy[ FSH_NL_ATTRIBUTE_LEN ] = {
    [FSH_NL_A_UNSPEC] = { .type = NLA_UNSPEC },
    [FSH_NL_A_TL_ID] = { .type = NLA_U32 },
    [FSH_NL_A_TL_ALLOW] = { .type = NLA_U8 },
};

static struct genl_family __fshNLFamily = {
    .name = FAMILY_NAME,
    .version = 1,
    .ops = __fshNLOps,
    .n_ops = 2,
    .policy = __fshNLPolicy,
    .maxattr = FSH_NL_ATTRIBUTE_LEN,
    .module = THIS_MODULE,
    .parallel_ops = 1,
};

static struct {
    uint32_t currentPortId;
    void ( *registerModuleCallback )( void );
} fshNLConnectionInfo = {
    .currentPortId = -1,
    .registerModuleCallback = NULL,
};

int fshNLRegisterDoit( struct sk_buff *senderSkb, struct genl_info *info )
{
    pr_info( "Received REGISTER message, registering %u\n", info->snd_portid );
    fshNLConnectionInfo.currentPortId = info->snd_portid;

    if ( fshNLConnectionInfo.registerModuleCallback != NULL ) {
        fshNLConnectionInfo.registerModuleCallback();
    }

    return 0;
}

int fshNLReplyDoit( struct sk_buff *senderSkb, struct genl_info *info )
{
    struct nlattr *attr;
    uint32_t id;
    uint8_t allow;

    attr = info->attrs[ FSH_NL_A_TL_ID ];
    if ( attr == NULL ) {
        pr_err( "fshNLReplyDoit(): missing id attribute\n" );
        return 0;
    }
    id = nla_get_u32( attr );

    attr = info->attrs[ FSH_NL_A_TL_ALLOW ];
    if ( attr == NULL ) {
        pr_err( "fshNLReplyDoit(): missing allow attribute\n" );
        return 0;
    }
    allow = nla_get_u8( attr );

    SetResponseForContext( id, allow );

    return 0;
}

void NetlinkInitConnection( void ( *registerModuleCallback )( void ) )
{
    int rc;

    fshNLConnectionInfo.registerModuleCallback = registerModuleCallback;

    rc = genl_register_family( &__fshNLFamily );
    if ( rc != 0 )
        pr_err( "NetlinkInitConnectlink(): failed to register family\n" );
}

void NetlinkUninitConnection( void )
{
    int rc;

    rc = genl_unregister_family( &__fshNLFamily );
    if ( rc != 0 )
        pr_err( "NetlinkUninitConnection(): failed to unregister family\n" );
}

int InitMessageHeaders( FSHookNLMsg *msg )
{
    msg->msg = genlmsg_new( NLMSG_GOODSIZE, GFP_KERNEL );
    if ( msg->msg == NULL )
        return -1;

    msg->head = genlmsg_put( msg->msg, fshNLConnectionInfo.currentPortId, 0, &__fshNLFamily, 0,
                             FSH_NL_C_MESSAGE );
    if ( msg->head == NULL )
        return -1;

    return 0;
}

int NetlinkSendMessage( FSHookNLMsg *msg )
{
    return genlmsg_unicast( &init_net, msg->msg, fshNLConnectionInfo.currentPortId );
}
