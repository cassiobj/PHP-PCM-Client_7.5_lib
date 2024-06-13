/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PHP-PCM-Client-lib
  PHP Extension interact with Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


#ifndef _PCM_STRUCTURE_FUNCTIONS
#define _PCM_STRUCTURE_FUNCTIONS

#include <stdio.h>
#include <stdbool.h>
#include "cm_fm.h"


#include "pin_mta.h"
#include "php.h"



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

#define __PROGRAM__  "PHP-PCM-Client"

//#include "ac_errs.h"
//#include "ac_custom_flds.h"


#include <pthread.h>
//#include "ac_types.h"

#include "time.h"





typedef struct
{
	char *name;
	char *descr;
	char *type;
	long number;
	int numberType;
	int status;	
	char  typeInformaton;
} field_spec ;

typedef struct
{
	char opcode[61];
	long number;	
} pcm_opcode ;


typedef struct {
	int32 chunksize;
	char *strp;
	int32 size;
	int32 strsize;
} xml_t;



typedef struct
{
	char *key;
	char *action;
	int index;
	char field[64];
	char type[64];
	int size;
	void *value;
	struct struct_flist *sub_flist;
	int64 totalSizeKey;
} struct_flist ;



typedef struct
{
	int	  tab;	
	char field[64];
	long int fieldNumber;
	char type[64];	
	void *value;	
	struct single_flist *array;
	struct single_flist *next;
	struct single_flist *head;
} single_flist ;

typedef struct
{
	char field[64];
	char type[64];
	int typeNumber;
	int index_array;
	int size;
	void *value;
	struct pcm_flist *sub_flist;
	int64 totalSizeKey;
	struct pcm_flist *next;
} pcm_flist ;



static pcm_context_t	*CTXP = NULL;
static pin_db_no_t		DATABASE;
extern pcm_opcode *opcodeList;
static pin_flist_t	*trans_flistp = NULL;
static pin_errbuf_t	    EBUF;


// extern pcm_flist


typedef struct
{
	pcm_context_t	*CTXP;
	pin_db_no_t		DATABASE;
	int				test;
	pcm_opcode *opcodeList;
	pin_flist_t	*trans_flistp;	
} PCM_ARRAY_CONN;



#define MAX_CONTEXT_CONN 16
#define MAX_DEPTH_FLIST 64

// PCM_ARRAY_CONN ARRAY_CONN[MAX_CONTEXT_CONN];
extern PCM_ARRAY_CONN ARRAY_CONN[MAX_CONTEXT_CONN];

extern int keyConn;
static int totalConn=0;




/*
#define PIN_FLDT_UNUSED     0
#define PIN_FLDT_INT        1
#define PIN_FLDT_UINT       2   // OBSOLETE
#define PIN_FLDT_ENUM       3
#define	PIN_FLDT_NUM        4   // OBSOLETE
#define PIN_FLDT_STR        5
#define PIN_FLDT_BUF        6
#define PIN_FLDT_POID       7
#define PIN_FLDT_TSTAMP     8
#define PIN_FLDT_ARRAY      9   // array element
#define PIN_FLDT_SUBSTRUCT  10  // sub-type substructure
#define	PIN_FLDT_OBJ        11  // whole object
#define PIN_FLDT_BINSTR     12  // (short) binary string data
#define PIN_FLDT_ERR        13
#define PIN_FLDT_DECIMAL    14
#define PIN_FLDT_TIME       15
#define PIN_FLDT_TEXTBUF    16
#define PIN_FLDT_ERRBUF     PIN_FLDT_ERR

#define PIN_FLDT_LAST       16
*/
static const char *const PCM_FIELD_TYPE[] = {
    "",
    "INT",
    "UINT",
	"ENUM",
	"NUM",
	"STR",
	"BUF",
	"POID",
	"TSTAMP",
	"ARRAY",
	"SUBSTRUCT",
	"OBJ",
	"BINSTR",
	"ERR",
	"DECIMAL",
	"TIME",
	"TEXTBUF",
	"ERR"
};


int addFlistElement(pcm_flist ***flist_in, char *field, char *type, int index_array, void *value);
int addFlistElement2(struct_flist ***flist_in, int64 line, char *key, char *action, int index, int field, int type, int size, void *value);
int destroyFlistElement(struct_flist ***flist_in);
char *getFieldType(char *fieldName);
int getXML(xmlNodePtr node, char **result, int *depth);
// void process_flist_array(pin_flist_t **flistp, zval *array, pin_errbuf_t *ebuf) ;





#endif
