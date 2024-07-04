
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
int keyConn;




PHP_FUNCTION(pcm_flist_set)
{
    zval *flist;
    
    char *field;
    size_t field_len;
    zend_long element_z;
    long element;
    
    zval *value = NULL;

    char msg[BUFSIZ * 16];

    zval result;
    array_init(&result);

    /*ZEND_PARSE_PARAMETERS_START(0, 1)
			Z_PARAM_OPTIONAL
			Z_PARAM_STRING(flist, s_field_str);
	ZEND_PARSE_PARAMETERS_END();*/

    /*if (zend_parse_parameters(ZEND_NUM_ARGS(), "az|z", &flist, &field, &field_len, &element, &value) == FAILURE) {
        RETURN_NULL();
    }*/

    ZEND_PARSE_PARAMETERS_START(4, 4)			
            Z_PARAM_ZVAL(flist);
			Z_PARAM_STRING(field, field_len);
            Z_PARAM_LONG(element_z);
            Z_PARAM_ZVAL(value);
	ZEND_PARSE_PARAMETERS_END();

    element = element_z;

    
/*
    add_assoc_string(&result, "field", field);
    add_assoc_string(&result, "type", "tipo 1");
    add_assoc_long(&result, "index_array", element);

    

    if (value) {
        switch (Z_TYPE_P(value)) {
            case IS_STRING: {
                //zval new_generic_string;
                // ZVAL_STR(&new_generic_string, Z_STR_P(value));
                zend_string  *new_generic_string;
                new_generic_string = Z_STR_P(value);
                add_assoc_string(&result, "value", (const char*) new_generic_string);                
                break;
            }
            case IS_LONG: {
                zval new_generic_long;
                ZVAL_LONG(&new_generic_long, Z_LVAL_P(value));
                add_assoc_long(&result, "value", (long ) &new_generic_long );
                break;
            }
            case IS_DOUBLE: {
                zval new_generic_double;
                double valor;                
                ZVAL_DOUBLE(&new_generic_double, Z_DVAL_P(value));
                valor = Z_DVAL_P(value);
                add_assoc_double(&result, "value",  valor );                
                break;
            }
            case IS_ARRAY: {
                zval new_generic_array;
                ZVAL_ARR(&new_generic_array, Z_ARR_P(value));                
                add_assoc_array(&result, "value", &new_generic_array);
                break;
            }
            default:
                sprintf(msg, __PROGRAM__ " pcm_flist_set().  error parsing value "  );
                php_error_docref(NULL, E_WARNING, msg);
                
                pin_set_err(&EBUF, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, (&EBUF)->pin_err, 0, 0, 0);
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg, &EBUF);		
		        
		        zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
                RETURN_THROWS();

                RETURN_FALSE;
        }
    }

    

        // Adiciona os dados do nó atual ao array de resultado
    add_next_index_zval(flist, result );

    fprintf(stderr, "teste 2\n");
    */

    RETURN_ZVAL(flist, 0, 1);
}
