
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


int pcm_get_poid_type(
	char 			*poid_str
	,char 			**poid_type
	,pin_errbuf_t	*ebuf);

int pcm_create_poid(
	pcm_context_t	*ctxp
	,int64			database
	,char			*type_str
	,pin_poid_id_t  id, 
	char			**poid_number 
	,pin_errbuf_t	*ebuf
) ;


pin_poid_id_t pcm_get_poid_number(
	char 			*poid_str
	,pin_errbuf_t	*ebuf
);


int64
pcm_get_poid_db(
	char 			*poid_str
	,pin_errbuf_t	*ebuf);


/* {{{ void pcm_get_poid_number() */
PHP_FUNCTION(pcm_get_poid_db)
{
	int64		poid_db;

	char *poid_str;
	size_t s_poid_str;
	

	char    msg[BUFSIZ];

	ZEND_PARSE_PARAMETERS_START(0, 1)
			Z_PARAM_OPTIONAL
			Z_PARAM_STRING(poid_str, s_poid_str);
	ZEND_PARSE_PARAMETERS_END();


	poid_db = pcm_get_poid_db(poid_str, &EBUF);
	if (poid_db == -1)	
	{
		sprintf(msg, __PROGRAM__" pcm_get_poid_db error getting database from poid %s \n", poid_str);
    	fprintf(stderr, msg);		
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);		
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        RETURN_THROWS();
		RETURN_NULL();
	}


	RETURN_LONG(poid_db);

}
/* }}} */


/* {{{ void pcm_get_poid_type() */
PHP_FUNCTION(pcm_get_poid_type)
{

	pcm_context_t	*ctxp = NULL;
	pin_errbuf_t	ebuf;
	int64		database;
	char		*poid_type;

	char *poid_str;
	size_t s_poid_str;

	char msg[BUFSIZ];

	ZEND_PARSE_PARAMETERS_START(0, 1)
			Z_PARAM_OPTIONAL
			Z_PARAM_STRING(poid_str, s_poid_str);
	ZEND_PARSE_PARAMETERS_END();


	
	if (pcm_get_poid_type(poid_str, &poid_type, &EBUF) == 1)
	{
		sprintf(msg, __PROGRAM__" pcm_get_poid_type error getting type from poid %s \n", poid_str);
    	fprintf(stderr, msg);		
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);		
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        RETURN_THROWS();

		RETURN_NULL();
	}
	


	RETURN_STRING(poid_type);

}
/* }}} */


/* {{{ void pcm_get_poid_number() */
PHP_FUNCTION(pcm_get_poid_number)
{

	pcm_context_t	*ctxp = NULL;
	pin_errbuf_t	ebuf;
	int64		database;
	pin_poid_id_t		poid_id;

	char *poid_str;
	size_t s_poid_str;

	char msg[BUFSIZ];

	ZEND_PARSE_PARAMETERS_START(0, 1)
			Z_PARAM_OPTIONAL
			Z_PARAM_STRING(poid_str, s_poid_str);
	ZEND_PARSE_PARAMETERS_END();


	poid_id = pcm_get_poid_number(poid_str, &EBUF);
	if (poid_id == 0)
	{
		sprintf(msg, __PROGRAM__" pcm_get_poid_number error getting number from poid %s \n", poid_str);
    	fprintf(stderr, msg);		
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);		
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        RETURN_THROWS();
		RETURN_NULL();
	}

	RETURN_LONG(poid_id);

}
/* }}} */




/* {{{ void pcm_get_poid_number() */
PHP_FUNCTION(pcm_create_poid)
{

	zend_long		zdatabase;
	int64			database;
	zend_long			zpoid_id;
	pin_poid_id_t		poid_id;
	char		*poid_number;

	char *type_str;
	size_t s_type_str;

	char     msg[BUFSIZ];

	ZEND_PARSE_PARAMETERS_START(3, 3)
			Z_PARAM_LONG(zdatabase);
			Z_PARAM_STRING(type_str, s_type_str);
			Z_PARAM_LONG(zpoid_id);
	ZEND_PARSE_PARAMETERS_END();

	poid_id = zpoid_id;
	database = zdatabase;

	if (pcm_create_poid(&CTXP, database, type_str, poid_id , &poid_number, &EBUF) == 1)
	{
		sprintf(msg, __PROGRAM__" pcm_create_poid error creating poid ");
    	fprintf(stderr, msg);		
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);		
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        RETURN_THROWS();
		RETURN_NULL();
	}


	RETURN_STRING(poid_number);

}
/* }}} */









/*=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=


=-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-==-=-=-=*/


int64
pcm_get_poid_db(
	char 			*poid_str
	,pin_errbuf_t	*ebuf)
{
	poid_t 	*poid_number;
	int64  retorno;
	char msg[256];
	
	PIN_ERR_CLEAR_ERR(ebuf);

	sprintf(msg, __PROGRAM__" pcm_get_poid_number from: %s", poid_str);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg);

	poid_number = PIN_POID_FROM_STR(poid_str, NULL , ebuf)	;
	if( PIN_ERR_IS_ERR( ebuf ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_get_poid_number error", ebuf);
		return (pin_poid_id_t) -1;
	}

	retorno = PIN_POID_GET_DB(poid_number);
	
	PIN_POID_DESTROY(poid_number, NULL);

	return retorno;
}


int 
pcm_get_poid_type(
	char 			*poid_str
	,char 			**poid_type
	,pin_errbuf_t	*ebuf)
{
	poid_t 	*poid_number;
	char  *type=0;
	char 	*retorno;
	char msg[256];
	
	PIN_ERR_CLEAR_ERR(ebuf);

	sprintf(msg, __PROGRAM__" pcm_get_poid_type from: %s", poid_str);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg);

	poid_number = PIN_POID_FROM_STR(poid_str, NULL , ebuf)	;
	if( PIN_ERR_IS_ERR( ebuf ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_get_poid_type error", ebuf);
		return ( -1 );
	}

	type = (char *)PIN_POID_GET_TYPE(poid_number);
	
		
	*poid_type = (char *) calloc ( (strlen(type) + 1), sizeof(char));
	strcpy(*poid_type, type);

	
	// PIN_POID_GET_TYPE
	PIN_POID_DESTROY(poid_number, NULL);

	return(0);
}



pin_poid_id_t
pcm_get_poid_number(
	char 			*poid_str
	,pin_errbuf_t	*ebuf)
{
	poid_t 	*poid_number;
	pin_poid_id_t  retorno;
	char msg[256];
	
	PIN_ERR_CLEAR_ERR(ebuf);

	sprintf(msg, __PROGRAM__" pcm_get_poid_number from: %s", poid_str);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg);

	poid_number = PIN_POID_FROM_STR(poid_str, NULL , ebuf)	;
	if( PIN_ERR_IS_ERR( ebuf ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_get_poid_number error", ebuf);
		return (pin_poid_id_t) 0;
	}

	retorno = PIN_POID_GET_ID(poid_number);
	// PIN_POID_GET_TYPE
	PIN_POID_DESTROY(poid_number, NULL);

	return retorno;
}




int 
pcm_create_poid(
	pcm_context_t	*ctxp
	,int64			database
	,char			*type_str
	,pin_poid_id_t  id, 
	char			**poid_number 
	,pin_errbuf_t	*ebuf
)
{
	poid_t			*poid_p;
	char msg[256];
	char 			*poid_str;

	int32	        maxpoidlen = 0;
	char	        poid_buf[PCM_MAX_POID_TYPE + 1];

	PIN_ERR_CLEAR_ERR(ebuf);

	sprintf(msg, __PROGRAM__" pcm_create_poid from: db: %lld, type: %s , id: %lld", database, type_str,  poid_str, id);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg);

	poid_p = PIN_POID_CREATE(database, type_str, id, ebuf);
	if( PIN_ERR_IS_ERR( ebuf ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_create_poid error in PIN_POID_CREATE ", ebuf);
		return ( 1 );
	}

	memset(poid_buf, 0, (PCM_MAX_POID_TYPE + 1));
	poid_str = poid_buf;
	maxpoidlen = sizeof(poid_buf);

	PIN_POID_TO_STR(poid_p, &poid_str, &maxpoidlen, ebuf)	
	if( PIN_ERR_IS_ERR( ebuf ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_create_poid error in PIN_POID_TO_STR ", ebuf);
		PIN_POID_DESTROY(poid_p, NULL);
		return ( 1 );
	}

	*poid_number = (char *) calloc ( (strlen(poid_str) + 1), sizeof(char));
	strcpy(*poid_number, poid_str);
	
		
	PIN_POID_DESTROY(poid_p, NULL);

	return(0);

}	