#include <stdio.h>




#include "pinlog.h"
#include "pin_errs.h"
#include "pcm.h"
#include "ops/base.h"
#include "pin_cust.h"

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "zend_attributes.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "zend_interfaces.h"
#include "zend_attributes.h"
#include "zend_ini.h"


#include "pcm_defines.h"


#include "pcm_structures.h"


PCM_ARRAY_CONN ARRAY_CONN[MAX_CONTEXT_CONN];
int keyConn;


void pcm_log_file(void)
{
	char		logfile[256];
	int32			*loglevel_p;
	int32			loglevel;
	char		*c_ptr = (char *)NULL;
	///char		*program = __PROGRAM__;
	int32		err;

	PIN_ERR_SET_PROGRAM(__PROGRAM__);
	PIN_ERR_SET_LEVEL(PIN_ERR_LEVEL_ERROR);


	memset(logfile, 0, 256);
	strcpy(logfile, "php-pcm-client.pinlog");
	pin_conf(__PROGRAM__, "logfile", PIN_FLDT_STR, (caddr_t *)&(c_ptr), &err);
	if (c_ptr != (char *)NULL) {
		pin_strlcpy(logfile, c_ptr, sizeof(logfile));
		pin_free(c_ptr);
		c_ptr = (char *)NULL;
	}
	PIN_ERR_SET_LOGFILE(logfile);



	pin_conf(__PROGRAM__, "loglevel", PIN_FLDT_INT, (caddr_t *)&loglevel_p, &err);
	if (loglevel_p != (int32 *)NULL) {
		loglevel = *loglevel_p;
		free(loglevel_p);
	}
	PIN_ERR_SET_LEVEL(loglevel);

	PIN_ERR_SET_LOGFILE(logfile);
}





