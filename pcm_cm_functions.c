
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
int keyConn;


int pcm_connect_cm(u_int64		*database);
int pcm_execute_op(	
	   long		flag,
		pin_flist_t		*in_flistpp,
		int opcode,
		pin_flist_t		**r_flistpp);



void
sample_read_obj_search(
	pcm_context_t	*ctxp,
	u_int64		database,
	pin_errbuf_t	*ebufp);

void
pcm_flist_execute(
	pcm_context_t	*ctxp,
	u_int64		database,
	pin_errbuf_t	*ebufp,
	char *flist);


int pcm_set_context_cm(int		keyContext, u_int64		*database );

static zval *flist_entrada = NULL;

/* {{{ void pcm_op() */
PHP_FUNCTION(pcm_op)
{

	pcm_context_t	*ctxp = NULL;
	pin_errbuf_t	ebuf;
	int64		database;

	char *c_flist;
	xml_t xml_buf;
	char *retorno;
	int opcode=0;
	zend_long zopcode = 0;
	zend_long zflag = 0;
	long pcm_flag = 0;
	char *flist_entrada_str;	
	size_t s_flist_entrada;

	zval *flist_entrada_array = NULL;
    

	char msg[BUFSIZ];
	
	struct_flist *array_retorno;

	pin_flist_t	*s_flistp = NULL;
	pin_flist_t	*r_flistp = NULL;


	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_LONG(zopcode);
		//Z_PARAM_STRING(flist_entrada, s_flist_entrada);
		Z_PARAM_ZVAL(flist_entrada);
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(zflag);
	ZEND_PARSE_PARAMETERS_END();

	opcode = zopcode;
	pcm_flag = zflag;

	
	
	if (Z_TYPE_P(flist_entrada) == IS_STRING) {
        flist_entrada_str = Z_STRVAL_P(flist_entrada);
        s_flist_entrada = Z_STRLEN_P(flist_entrada);
        // Processa flist_entrada como string

		c_flist = (char *) calloc ( (s_flist_entrada + 1), sizeof(char));
		memset(c_flist, 0, (s_flist_entrada + 1));
		//sprintf(c_flist, "0 PIN_FLD_POID           POID [0] 0.0.0.1 /bill 157240 0\n");
		sprintf(c_flist, flist_entrada);
    } 
	else if (Z_TYPE_P(flist_entrada) == IS_ARRAY) 
	{
		single_flist *flist_tree = NULL;
		
		int			 indice_array = 0;

		

        flist_entrada_array = flist_entrada;

		
		process_flist_array( flist_entrada_array, &flist_tree, 0) ;

		
		if (flist_tree!=NULL);
			flist_tree = flist_tree->head;

		PIN_ERRBUF_CLEAR( &EBUF );
		

		s_flistp = PIN_FLIST_CREATE(&EBUF);

		convert_array_flist(&s_flistp, flist_tree);

		// PIN_FLIST_PRINT(s_flistp, stdout, &EBUF);

		// fprintf(stderr, "Chegou Tipo %d\n", Z_TYPE_P(flist_entrada));
    } 
    else 
	{               
		sprintf(msg, __PROGRAM__ " pc_op(). The second parameter must be either a string or an array");
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        RETURN_THROWS();
		RETURN_NULL();		
    }

	
	PIN_ERR_CLEAR_ERR(&EBUF);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pc_op()");
	//fprintf(stderr, "pcm_op antes: Database %ld , alocacado:%d \n\n", ARRAY_CONN[keyConn].DATABASE, ARRAY_CONN[keyConn].test);
	if (pcm_connect_cm( &(ARRAY_CONN[keyConn]).DATABASE  ) < 0)
	{		
		sprintf(msg, __PROGRAM__ " pc_op(). Error connecting CM database %ld . pin_err: ", ARRAY_CONN[keyConn].DATABASE );
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        RETURN_THROWS();
		RETURN_NULL();		
	}
	//fprintf(stderr, "pcm_op depois: Database %ld , alocacado:%d \n\n", ARRAY_CONN[keyConn].DATABASE, ARRAY_CONN[keyConn].test);

	if (Z_TYPE_P(flist_entrada) == IS_STRING) 
	{
		PIN_STR_TO_FLIST(c_flist, ARRAY_CONN[keyConn].DATABASE , &s_flistp, &EBUF);
		if( PIN_ERR_IS_ERR( &EBUF ))
		{

			PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_op: error converting str to flist", &EBUF);		
			sprintf(msg, __PROGRAM__ " pcm_op: error converting str to flist  "  );
			zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
			RETURN_THROWS();

			RETURN_NULL();
		}
	}
	//PIN_FLIST_PRINT(s_flistp, NULL, &EBUF);

	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, c_flist );
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_op flist in: ",s_flistp);

	if (pcm_execute_op( pcm_flag, s_flistp, opcode, &r_flistp) < 0)
	{
		PIN_FLIST_DESTROY_EX(&s_flistp, &EBUF);
		PIN_FLIST_DESTROY_EX(&r_flistp, &EBUF);
		sprintf(msg, __PROGRAM__ " pc_op(). Error Executing Opcode. pin_err: "  );
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        RETURN_THROWS();
		RETURN_NULL();	
		
	}
	

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_op out: ",r_flistp);

	// convertendo flist de saida para xml
	PIN_FLIST_TO_XML( r_flistp, PIN_XML_BY_NAME, 0, &xml_buf.strp, &xml_buf.strsize, "PCM_CLIENT", &EBUF );
	if( PIN_ERR_IS_ERR( &EBUF ))
	{
		PIN_FLIST_DESTROY_EX(&s_flistp, &EBUF);
		PIN_FLIST_DESTROY_EX(&r_flistp, &EBUF);		
	
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_op: error converting flist to xml", &EBUF);		
		sprintf(msg, __PROGRAM__ " pc_op().  converting flist to xml  "  );
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        RETURN_THROWS();

		RETURN_NULL();
	}

	retorno = (char *) calloc ( (xml_buf.strsize + 1), sizeof(char));
	memset(retorno, 0, (xml_buf.strsize + 1));
	strncpy(retorno, xml_buf.strp, xml_buf.strsize);


	PIN_FLIST_DESTROY_EX(&r_flistp, &EBUF);
	PIN_FLIST_DESTROY_EX(&s_flistp, &EBUF);

	RETURN_STRING( retorno );

}
/* }}} */






/* {{{ void pcm_set_context() */
PHP_FUNCTION(pcm_set_context)
{


	char *c_flist;
	xml_t xml_buf;
	char *retorno;
	int keyContext=0;
	zend_long zkeyContext = 0;
	zend_long zdatabase = 0;
	long pcm_flag = 0;
	char *flist_entrada;
	size_t s_flist_entrada;
	uint64_t	database=0;

	
	struct_flist *array_retorno;

	pin_flist_t	*s_flistp = NULL;
	pin_flist_t	*r_flistp = NULL;


	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(zkeyContext);		
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(zdatabase);		
	ZEND_PARSE_PARAMETERS_END();

	keyConn = zkeyContext;
	database = zdatabase;
	
	PIN_ERR_CLEAR_ERR(&EBUF);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pc_set_context()");

	
	RETURN_LONG( pcm_set_context_cm(keyConn, database) );

}
/* }}} */




/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
FUNCOES
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

int pcm_connect_cm(u_int64		*database)
{

	PIN_ERR_CLEAR_ERR(&EBUF);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_connect_cm: function called");

	/***********************************************************
	* Open the database context
	***********************************************************/
	
	//fprintf(stderr, "Conecando %d database %ld \n\n", keyConn,  ARRAY_CONN[keyConn].DATABASE);
	ARRAY_CONN[keyConn].test=0;
	if ( ARRAY_CONN[keyConn].CTXP == NULL)
	{
		//fprintf(stderr, " 2 Conecando %d database %ld \n\n", keyConn,  ARRAY_CONN[keyConn].DATABASE);
		PCM_CONNECT(&(ARRAY_CONN[keyConn]).CTXP, database, &EBUF);
		if( PIN_ERR_IS_ERR( &EBUF ))
		{			
			PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_connect_cm: error connection", &EBUF);
			fprintf(stderr, __PROGRAM__" pcm_connect_cm: error connection\n");
			return -1;
		}
		ARRAY_CONN[keyConn].test=1;
	}
	//else
	//	fprintf(stderr, "ja conectado em  %d database %ld \n\n", keyConn,  ARRAY_CONN[keyConn].DATABASE);


	if( PIN_ERR_IS_ERR( &EBUF ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_connect_cm: error connection", &EBUF);
		return -1;
	}

	return 0;

}



void pcm_disconnect_cm(pcm_context_t	**ctxp,
		u_int64		*database,
		pin_errbuf_t	*ebuf)
{

	PIN_ERR_CLEAR_ERR(ebuf);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_disconnect_cm: function called");

	/***********************************************************
	* Close the database context
	***********************************************************/
	//pcm_connect_cm(ctxp, database, &ebuf);
	PCM_CONTEXT_CLOSE(*ctxp, *database, ebuf);


	if( PIN_ERR_IS_ERR( ebuf ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_disconnect_cm: error connection", ebuf);
	}



}




int pcm_execute_op(	
	   long		flag,
		pin_flist_t		*in_flistpp,
		int opcode,
		pin_flist_t		**r_flistpp)
{
	pin_flist_t    *ret_flist = NULL;



	PIN_ERR_CLEAR_ERR(&EBUF);


	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_execute_op flist in: ",in_flistpp);

	//if (CTXP==NULL)
	
	
	pcm_connect_cm( &(ARRAY_CONN[keyConn]).DATABASE );

	PCM_OP( ARRAY_CONN[keyConn].CTXP , opcode, flag, in_flistpp, &ret_flist, &EBUF);
	if( PIN_ERR_IS_ERR( &EBUF ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_execute_op: error connection", &EBUF);
		return -1;
	}

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_execute_op flist out: ",ret_flist);

	*r_flistpp = ret_flist;

	/*
	pcm_disconnect_cm(ctxp, database, ebuf);
	ctxp = NULL;
	*/

	PIN_ERR_CLEAR_ERR(&EBUF);

	return 0;

}






void
pcm_flist_execute(
	pcm_context_t	*ctxp,
	u_int64		database,
	pin_errbuf_t	*ebufp,
	char *flist)
{
	pin_flist_t	*flistp = NULL;
	pin_flist_t	*a_flistp = NULL;
	pin_flist_t	*r_flistp = NULL;

	poid_t		*objp = NULL;
	u_int64		id;


	/***********************************************************
	 * Check the error buffer.
	 ***********************************************************/
	if (PIN_ERR_IS_ERR(ebufp))
		return;
	PIN_ERR_CLEAR_ERR(ebufp);

	/***********************************************************
	 * Allocate the flist for searching.
	 ***********************************************************/
	flistp = PIN_FLIST_CREATE(ebufp);

	/***********************************************************
	 * Allocate the search poid and give it to the flist.
	 ***********************************************************/
	id = (u_int64)201;

	objp = PIN_POID_CREATE(database, "/search", id, ebufp);
	PIN_FLIST_FLD_PUT(flistp, PIN_FLD_POID, (void *)objp, ebufp);

	/***********************************************************
	 * Add a search arguments array.
	 ***********************************************************/
	a_flistp = PIN_FLIST_ELEM_ADD(flistp, PIN_FLD_ARGS, 1, ebufp);

	/***********************************************************
	 * Allocate the master account poid and add as the arg.
	 ***********************************************************/
	id = (u_int64)1;

	objp = PIN_POID_CREATE(database, "/account", id, ebufp);
	PIN_FLIST_FLD_PUT(a_flistp, PIN_FLD_POID, (void *)objp, ebufp);

	/***********************************************************
	 * Add a search results marker.
	 *
	 * Making this NULL forces the read-object result we want.
	 ***********************************************************/
	a_flistp = (pin_flist_t *)NULL;
	PIN_FLIST_ELEM_PUT(flistp, a_flistp, PIN_FLD_RESULTS, 0, ebufp);

	/***********************************************************
	 * Call the DM to do the search.
	 ***********************************************************/
	PCM_OP(ctxp, PCM_OP_SEARCH, 0, flistp, &r_flistp, ebufp);

	/***********************************************************
	 * Get (and show) the results.
	 ***********************************************************/
	a_flistp = PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_RESULTS, 0, 0, ebufp);

	fprintf(stdout,"\nSimple Search Result Flist:\n");
	PIN_FLIST_PRINT(a_flistp, stdout, ebufp);

	/***********************************************************
	 * Clean up.
	 ***********************************************************/
	/* Free local memory */
	PIN_FLIST_DESTROY(r_flistp, NULL);
	PIN_FLIST_DESTROY(flistp, NULL);

	/* Error? */
	if (PIN_ERR_IS_ERR(ebufp)) {
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
			"sample_read_obj_search error", ebufp);
	}

	return;
}






/*search test function*/
void
sample_read_obj_search(
	pcm_context_t	*ctxp,
	u_int64		database,
	pin_errbuf_t	*ebufp)
{
	pin_flist_t	*flistp = NULL;
	pin_flist_t	*a_flistp = NULL;
	pin_flist_t	*r_flistp = NULL;

	poid_t		*objp = NULL;
	u_int64		id;


	/***********************************************************
	 * Check the error buffer.
	 ***********************************************************/
	if (PIN_ERR_IS_ERR(ebufp))
		return;
	PIN_ERR_CLEAR_ERR(ebufp);

	/***********************************************************
	 * Allocate the flist for searching.
	 ***********************************************************/
	flistp = PIN_FLIST_CREATE(ebufp);

	/***********************************************************
	 * Allocate the search poid and give it to the flist.
	 ***********************************************************/
	id = (u_int64)1;

	objp = PIN_POID_CREATE(database, "/search", id, ebufp);
	PIN_FLIST_FLD_PUT(flistp, PIN_FLD_POID, (void *)objp, ebufp);

	/***********************************************************
	 * Add a search arguments array.
	 ***********************************************************/
	a_flistp = PIN_FLIST_ELEM_ADD(flistp, PIN_FLD_ARGS, 1, ebufp);

	/***********************************************************
	 * Allocate the master account poid and add as the arg.
	 ***********************************************************/
	id = (u_int64)1;

	objp = PIN_POID_CREATE(database, "/account", id, ebufp);
	PIN_FLIST_FLD_PUT(a_flistp, PIN_FLD_POID, (void *)objp, ebufp);

	/***********************************************************
	 * Add a search results marker.
	 *
	 * Making this NULL forces the read-object result we want.
	 ***********************************************************/
	a_flistp = (pin_flist_t *)NULL;
	PIN_FLIST_ELEM_PUT(flistp, a_flistp, PIN_FLD_RESULTS, 0, ebufp);

	/***********************************************************
	 * Call the DM to do the search.
	 ***********************************************************/
	PCM_OP(ctxp, PCM_OP_SEARCH, 0, flistp, &r_flistp, ebufp);

	/***********************************************************
	 * Get (and show) the results.
	 ***********************************************************/
	a_flistp = PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_RESULTS, 0, 0, ebufp);

	fprintf(stdout,"\nSimple Search Result Flist:\n");
	PIN_FLIST_PRINT(a_flistp, stdout, ebufp);

	/***********************************************************
	 * Clean up.
	 ***********************************************************/
	/* Free local memory */
	PIN_FLIST_DESTROY(r_flistp, NULL);
	PIN_FLIST_DESTROY(flistp, NULL);

	/* Error? */
	if (PIN_ERR_IS_ERR(ebufp)) {
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
			"sample_read_obj_search error", ebufp);
	}

	return;
}




int pcm_set_context_cm(int		keyContext, u_int64		*database )
{
	char msg[BUFSIZ];

	PIN_ERR_CLEAR_ERR(&EBUF);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" pcm_connect_cm: function called");

	/***********************************************************
	* Open the database context
	***********************************************************/
	//pcm_connect_cm(ctxp, database, &ebuf);
	//static int keyConn=0;
	//static int totalConn=0;
	if ( (keyContext < 0) || 
		(keyContext > (MAX_CONTEXT_CONN-1) ) 
	)
	{
		sprintf(msg, __PROGRAM__" pcm_set_context_cm invalid value :%d \n Values must be between 0 and %d", keyContext, (MAX_CONTEXT_CONN-1));
    	fprintf(stderr, msg);
		pin_set_err(&EBUF, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, PIN_ERR_NOT_SUPPORTED, 0, 0, 0);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);
		return -1;

	}


	if (pcm_connect_cm( &(ARRAY_CONN[keyConn]).DATABASE  ) < 0)
	{
		return -1;		
	}
	
	if( PIN_ERR_IS_ERR( &EBUF ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_connect_cm: error connection", &EBUF);
		return -1;
	}

	return 0;

}






