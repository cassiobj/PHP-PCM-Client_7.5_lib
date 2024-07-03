#ifndef PCM_DEFINES_H
#define PCM_DEFINES_H



#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <stdio.h>
#include "pinlog.h"
#include "pin_errs.h"
#include "pcm.h"
#include "ops/base.h"
#include "ops/ar.h"
#include "ops/apn.h"
#include "ops/act.h"
#include "ops/bal.h"
#include "ops/bill.h"
#include "ops/collections.h"
#include "ops/cust.h"
#include "ops/delivery.h"
#include "ops/deposit.h"
#include "ops/ece.h"
#include "ops/gel.h"
#include "ops/gl.h"
#include "ops/gprs_aaa.h"
#include "ops/gprs.h"
#include "ops/gsm.h"
#include "ops/group.h"
#include "ops/filter_set.h"
#include "ops/infmgr.h"
#include "ops/inv.h"
#include "ops/ip.h"
#include "ops/mail.h"
#include "ops/monitor.h"
#include "ops/num.h"
#include "ops/offer_profile.h"
#include "ops/order.h"
#include "ops/perm.h"
#include "ops/price.h"
#include "ops/process_audit.h"
#include "ops/prov.h"
#include "ops/publish.h"
#include "ops/pymt.h"
#include "ops/rate.h"
#include "ops/remit.h"
#include "ops/rerate.h"
#include "ops/sat.h"
#include "ops/sdk.h"
#include "ops/sim.h"
#include "ops/suspense.h"
#include "ops/subscription.h"
#include "ops/tcf.h"
#include "ops/term.h"
#include "ops/ums.h"
#include "ops/voucher.h"
#include "ops/zonemap.h"
//#include "ops/infmgr.h"
//#include "ops/infmgr.h"
#include "pin_cust.h"


#include <libxml/parser.h>
#include <libxml/tree.h>

#include "pcm_structures.h"
#include "pcm_fields_ops_logic.h"

zend_class_entry *pcm_exception_ce;

#endif