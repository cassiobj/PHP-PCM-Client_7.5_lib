/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PHP-PCM-Client-lib
  PHP Extension interact with Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <signal.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pcm_defines.h"
#include "zend_exceptions.h"
#include "php_pcm_client.h"

#include "pcm_defines.h"

PCM_ARRAY_CONN ARRAY_CONN[MAX_CONTEXT_CONN];


/* {{{ long test2( [ string $var ] ) */
PHP_FUNCTION(pcm_open_transaction)
{
    zval result;
    zend_long zpcm_op_flag = 0;
	long pcm_op_flag = 0;
	char msg[BUFSIZ];
	char *flist_entrada;
	size_t s_flist_entrada = 0;
	char *trans_flist;	
	poid_t		*x_poid = NULL;

	//pin_flist_t	*in_flistp = NULL;
	pin_flist_t	*out_flistp = NULL;



	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(zpcm_op_flag)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(flist_entrada, s_flist_entrada);
	ZEND_PARSE_PARAMETERS_END();

	pcm_op_flag =  zpcm_op_flag;
	
	if ((pcm_op_flag != PCM_TRANS_OPEN_READONLY &&
	    pcm_op_flag != PCM_TRANS_OPEN_READWRITE &&
		pcm_op_flag != PCM_TRANS_OPEN_GLOBALTRANSACTION &&
		pcm_op_flag != PCM_OPFLG_USE_GIVEN_POID_DB_NUMBER &&
		pcm_op_flag != PCM_TRANS_OPEN_LOCK_OBJ &&
		pcm_op_flag != PCM_TRANS_OPEN_LOCK_DEFAULT &&
		pcm_op_flag !=  PCM_OPFLG_LOCK_NONE	) || 
		(s_flist_entrada ==0 && pcm_op_flag == PCM_TRANS_OPEN_LOCK_OBJ) 
	)
	{
    	sprintf(msg, __PROGRAM__" pcm_open_transaction invalid flag:%ld \n", pcm_op_flag);
    	fprintf(stderr, msg);
		pin_set_err(&EBUF, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_NOT_SUPPORTED, 0, 0, 0);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);		
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        RETURN_THROWS();
		RETURN_LONG(-1);
	}		

	
	PIN_ERR_CLEAR_ERR(&EBUF);
	
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_open_transaction()");

    //fprintf(stderr, "open_transaction antes: Database %ld , alocacado:%d \n\n", ARRAY_CONN[keyConn].DATABASE, ARRAY_CONN[keyConn].test);
    if (ARRAY_CONN[keyConn].CTXP == NULL)
    {
        if (pcm_connect_cm(  &(ARRAY_CONN[keyConn]).DATABASE ) < 0)        
        {
            PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_open_transaction(). Error conecting CM ");
			sprintf(msg, __PROGRAM__ " pcm_open_transaction(). Error conecting CM " );
		    zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
            RETURN_THROWS();
            RETURN_NULL();		
        }
    }
    //fprintf(stderr, "open_transaction depois: Database %ld , alocacado:%d \n\n", ARRAY_CONN[keyConn].DATABASE, ARRAY_CONN[keyConn].test);
	//return 0;


	if ( s_flist_entrada == 0)	   
	{
		
		x_poid = PIN_POID_CREATE(ARRAY_CONN[keyConn].DATABASE , "/dummy", -1, &EBUF);
	
		ARRAY_CONN[keyConn].trans_flistp = PIN_FLIST_CREATE(&EBUF);
		PIN_FLIST_FLD_SET(ARRAY_CONN[keyConn].trans_flistp, PIN_FLD_POID, x_poid, &EBUF);
		if( PIN_ERR_IS_ERR( &EBUF ))
		{
			PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, &EBUF);
			
			sprintf(msg, __PROGRAM__" pcm_open_transaction error creating flist. \n");
			fprintf(stderr, msg);
			pin_set_err(&EBUF, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, (&EBUF)->pin_err, 0, 0, 0);
			PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);			
		    zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
            RETURN_THROWS();
			RETURN_LONG(-1);
		}


	}
	else
	{
		trans_flist = (char *) calloc ( (s_flist_entrada + 1), sizeof(char));
		memset(trans_flist, 0, (s_flist_entrada + 1));
		//sprintf(c_flist, "0 PIN_FLD_POID           POID [0] 0.0.0.1 /bill 157240 0\n");
		sprintf(trans_flist, flist_entrada);
		PIN_STR_TO_FLIST(trans_flist, ARRAY_CONN[keyConn].DATABASE , &(ARRAY_CONN[keyConn]).trans_flistp, &EBUF);
		if( PIN_ERR_IS_ERR( &EBUF ))
		{
			

			sprintf(msg, __PROGRAM__"  pcm_open_transaction: error converting flist string to flist. \n");
			fprintf(stderr, msg);
			pin_set_err(&EBUF, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, (&EBUF)->pin_err, 0, 0, 0);
			PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);
			
		    zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
            RETURN_THROWS();
			RETURN_LONG(-1);		
		}
	}

	
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_open_transaction flist in: ", ARRAY_CONN[keyConn].trans_flistp);		
	if (pcm_execute_op(pcm_op_flag, ARRAY_CONN[keyConn].trans_flistp, PCM_OP_TRANS_OPEN, &out_flistp) < 0)
	{
		PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, &EBUF);
		PIN_FLIST_DESTROY_EX(&out_flistp, &EBUF);

		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_open_transaction flist out: ", out_flistp);
		sprintf(msg, __PROGRAM__"  pcm_open_transaction: error executing opcode. \n");
		fprintf(stderr, msg);
		pin_set_err(&EBUF, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, (&EBUF)->pin_err, 0, 0, 0);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);
		
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
		RETURN_THROWS();
		RETURN_LONG(-1);		
	}
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_open_transaction flist out: ",out_flistp);


	
    // PIN_FLIST_DESTROY_EX(&in_flistp, &EBUF);	
	PIN_FLIST_DESTROY_EX(&out_flistp, &EBUF);

    RETURN_LONG(0);
}






PHP_FUNCTION(pcm_commit_transaction)
{
    zval result;
    zend_long zpcm_op_flag = 0;
	long pcm_op_flag = 0;
	char msg[BUFSIZ];
	char *flist_entrada;
	size_t s_flist_entrada = 0;
	char *trans_flist;	
	poid_t		*x_poid = NULL;

	//pin_flist_t	*in_flistp = NULL;
	pin_flist_t	*out_flistp = NULL;



	ZEND_PARSE_PARAMETERS_NONE();

	
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_commit_transaction flist in: ", ARRAY_CONN[keyConn].trans_flistp);	
	if (pcm_execute_op( 0, ARRAY_CONN[keyConn].trans_flistp, PCM_OP_TRANS_COMMIT, &out_flistp) < 0)
	{
		PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, &EBUF);
		PIN_FLIST_DESTROY_EX(&out_flistp, &EBUF);

		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_commit_transaction flist out: ", out_flistp);
		sprintf(msg, __PROGRAM__"  pcm_commit_transaction: error executing opcode. \n");
		fprintf(stderr, msg);
		pin_set_err(&EBUF, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, (&EBUF)->pin_err, 0, 0, 0);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);		
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
		RETURN_THROWS();
		RETURN_LONG(-1);		
	}
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_commit_transaction flist out: ",out_flistp);


	
    PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, &EBUF);
	PIN_FLIST_DESTROY_EX(&out_flistp, &EBUF);

    RETURN_LONG(0);
}


PHP_FUNCTION(pcm_abort_transaction)
{
    zval result;
    zend_long zpcm_op_flag = 0;
	long pcm_op_flag = 0;
	char msg[BUFSIZ];
	char *flist_entrada;
	size_t s_flist_entrada = 0;
	char *trans_flist;	
	poid_t		*x_poid = NULL;

	//pin_flist_t	*in_flistp = NULL;
	pin_flist_t	*out_flistp = NULL;



	ZEND_PARSE_PARAMETERS_NONE();

	
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_abort_transaction flist in: ", ARRAY_CONN[keyConn].trans_flistp);	
	if (pcm_execute_op( 0, ARRAY_CONN[keyConn].trans_flistp, PCM_OP_TRANS_ABORT, &out_flistp) < 0)
	{
		PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, &EBUF);
		PIN_FLIST_DESTROY_EX(&out_flistp, &EBUF);

		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_abort_transaction flist out: ", out_flistp);
		sprintf(msg, __PROGRAM__"  pcm_abort_transaction: error executing opcode. \n");
		fprintf(stderr, msg);
		pin_set_err(&EBUF, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, (&EBUF)->pin_err, 0, 0, 0);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
		RETURN_THROWS();
		RETURN_LONG(-1);		
	}
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_abort_transaction flist out: ",out_flistp);


	
    PIN_FLIST_DESTROY_EX(&(ARRAY_CONN[keyConn]).trans_flistp, &EBUF);
	PIN_FLIST_DESTROY_EX(&out_flistp, &EBUF);

    RETURN_LONG(0);
}


