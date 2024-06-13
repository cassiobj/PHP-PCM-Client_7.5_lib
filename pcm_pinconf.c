
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <signal.h>




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


#include "php_pcm_client.h"


#include "pcm_defines.h"



int pcm_get_int_pin_conf(char *program, char *token, int *retorno  );
int pcm_get_poid_pin_conf(char *program, char *token, char **retorno );
int pcm_get_decimal_pin_conf(char *program, char *token, long double *retorno );
int pcm_get_string_pin_conf(char *program, char *token, char **retorno );



/* {{{ void pcm_pin_conf() */
PHP_FUNCTION(pcm_pin_conf)
{

	char *program_str;
	size_t s_program_str;
	char *token_str;
	size_t s_token_str;
	char *val_type_str;
	size_t s_val_type_str;

	char *retorno;
	int  value=0;
	long double decimal = 0;

	pin_poid_id_t		poid_id;
	char				*poid_type;
	int64				db = 0;

	char 				msg[BUFSIZ];

	ZEND_PARSE_PARAMETERS_START(3, 3)			
			Z_PARAM_STRING(program_str, s_program_str);
			Z_PARAM_STRING(token_str, s_token_str);
			Z_PARAM_STRING(val_type_str, s_val_type_str);			
	ZEND_PARSE_PARAMETERS_END();


	if (strncmp(val_type_str, "STR", s_val_type_str) ==0)
	{
		if (pcm_get_string_pin_conf(program_str, token_str, &retorno) == -1)
		{
			PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_pin_conf: error getting STR pin.conf information", &EBUF);
			sprintf(msg, __PROGRAM__ " pcm_pin_conf():  error getting STR pin.conf information "  );
			zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        	RETURN_THROWS();
			RETURN_NULL();
		}
		else
		{
			RETURN_STRING(retorno);
		}		
	}
	else if (strncmp(val_type_str, "INT", s_val_type_str) ==0)
	{
		if (pcm_get_int_pin_conf(program_str, token_str, &value) == -1)
		{
			PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_pin_conf: error getting INT pin.conf information", &EBUF);
			sprintf(msg, __PROGRAM__ " pcm_pin_conf: error getting INT pin.conf information "  );
			zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        	RETURN_THROWS();
			RETURN_NULL();
		}
		else
		{
			RETURN_LONG(value);
		}		
	}
	else if (strncmp(val_type_str, "DECIMAL", s_val_type_str) ==0)
	{
		if (pcm_get_decimal_pin_conf(program_str, token_str, &decimal) == -1)
		{
			PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_pin_conf: error getting DECIMAL pin.conf information", &EBUF);
			sprintf(msg, __PROGRAM__ " pcm_pin_conf: error getting DECIMAL pin.conf information "  );
			zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        	RETURN_THROWS();
			RETURN_NULL();
		}
		else
		{
			RETURN_DOUBLE(decimal);
		}		
	}
	else if (strncmp(val_type_str, "POID", s_val_type_str) ==0)
	{
		


		if (pcm_get_string_pin_conf(program_str, token_str, &retorno) == -1)
		{
			PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_pin_conf: error getting DECIMAL pin.conf information", &EBUF);
			sprintf(msg, __PROGRAM__ " pcm_pin_conf: error getting DECIMAL pin.conf information "  );
			zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        	RETURN_THROWS();
			RETURN_NULL();
		}
		else
		{
			zval retValPoid;
    		array_init(&retValPoid);

			
			if (
			   ( (db = pcm_get_poid_db(retorno, &EBUF)) == -1) ||
			   (pcm_get_poid_type(retorno, &poid_type, &EBUF) == -1) ||
			   ( (poid_id = pcm_get_poid_number(retorno, &EBUF)) == -1) )
			{
				PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_pin_conf: invalid poid value ", &EBUF);
				sprintf(msg, __PROGRAM__ " pcm_pin_conf: invalid poid value "  );
				zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        		RETURN_THROWS();
				RETURN_NULL();
			}
			
			/*if (pcm_get_poid_type(retorno, &poid_type, &EBUF) == -1)			
				RETURN_NULL();
			
			if ( (poid_id = pcm_get_poid_number(retorno, &EBUF)) == -1)
				RETURN_NULL();*/

			

			add_assoc_long(&retValPoid, "db", db);
			add_assoc_string(&retValPoid, "type", poid_type );
			add_assoc_long(&retValPoid, "id", poid_id );


			RETURN_ZVAL(&retValPoid, 0, 1);
		}		
	}
	else
	{
		sprintf(msg, __PROGRAM__ " pcm_pin_conf: invalid poid val type %s ", val_type_str  );
		zend_throw_exception(pcm_exception_ce, msg , 99);
		RETURN_THROWS();
		RETURN_NULL();
	}
	
		


	RETURN_NULL();

}
/* }}} */


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/




int pcm_get_string_pin_conf(char *program, char *token, char **retorno )
{
	char		*c_ptr = (char *)NULL;
	///char		*program = __PROGRAM__;
	int32		err;
	char 		buffer[BUFSIZ+1];

	memset(buffer, 0, BUFSIZ+1);
	
	pin_conf(program, token, PIN_FLDT_STR, (caddr_t *)&(c_ptr), &((&EBUF)->pin_err) );
	if (c_ptr != (char *)NULL) 
	{		
		
		pin_strlcpy(buffer, c_ptr, sizeof(buffer));
		
		*retorno = (char *) malloc (strlen(buffer) + 1);
		strcpy(*retorno, buffer);

		pin_free(c_ptr);
		
		c_ptr = (char *)NULL;
	}
	else
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_WARNING, __PROGRAM__" pcm_get_string_pin_conf: no value found", &EBUF);
		//fprintf(stderr,  __PROGRAM__" pcm_get_string_pin_conf: no value found for %s : %s\n", program, token);		
		return -1;
	}

	
	return 0;
}



int pcm_get_decimal_pin_conf(char *program, char *token, long double *retorno )
{
	char		*c_ptr = (char *)NULL;
	///char		*program = __PROGRAM__;
	int32		err;
	char 		buffer[BUFSIZ+1];

	memset(buffer, 0, BUFSIZ+1);
	
	pin_conf(program, token, PIN_FLDT_STR, (caddr_t *)&(c_ptr), &((&EBUF)->pin_err) );
	if (c_ptr != (char *)NULL) 
	{		
		
		pin_strlcpy(buffer, c_ptr, sizeof(buffer));
		
		//*retorno = (char *) malloc (strlen(buffer) + 1);
		//strcpy(*retorno, buffer);
		*retorno = atof(buffer);

		pin_free(c_ptr);
		
		c_ptr = (char *)NULL;
	}
	else
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_get_decimal_pin_conf: no value found", &EBUF);
		fprintf(stderr,  __PROGRAM__" pcm_get_decimal_pin_conf: no value found for %s : %s\n", program, token);		
		return -1;
	}

	
	return 0;
}



int pcm_get_poid_pin_conf(char *program, char *token, char **retorno )
{
	char		*c_ptr = (char *)NULL;
	///char		*program = __PROGRAM__;
	int32		err;
	char 		buffer[BUFSIZ+1];

	memset(buffer, 0, BUFSIZ+1);
	
	pin_conf(program, token, PIN_FLDT_STR, (caddr_t *)&(c_ptr), &((&EBUF)->pin_err) );
	if (c_ptr != (char *)NULL) 
	{		
		
		pin_strlcpy(buffer, c_ptr, sizeof(buffer));
		
		*retorno = (char *) malloc (strlen(buffer) + 1);
		strcpy(*retorno, buffer);

		pin_free(c_ptr);
		
		c_ptr = (char *)NULL;
	}
	else
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_get_poid_pin_conf: no value found", &EBUF);
		fprintf(stderr,  __PROGRAM__" pcm_get_poid_pin_conf: no value found for %s : %s\n", program, token);		
		return -1;
	}

	
	return 0;
}


int pcm_get_int_pin_conf(char *program, char *token, int *retorno  )
{
	char		logfile[256];
	int32			*value_p;
	
	char		*c_ptr = (char *)NULL;
	///char		*program = __PROGRAM__;
	int32		err;

	
	pin_conf(program, token, PIN_FLDT_INT, (caddr_t *)&value_p, &((&EBUF)->pin_err) );
	if (value_p != (int32 *)NULL) {
		*retorno = *value_p;
		free(value_p);
	}
	else
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_get_int_pin_conf: no value found", &EBUF);
		fprintf(stderr,  __PROGRAM__" pcm_get_int_pin_conf: no value found for %s : %s\n", program, token);		
		return -1;
	}

	return 0;
	
	
}

