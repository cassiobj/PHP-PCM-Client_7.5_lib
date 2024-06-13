/* pcm_client extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif



#include "php.h"
#include "ext/standard/info.h"
#include "php_pcm_client.h"
#include "pcm_client_arginfo.h"

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


#define AAAAAAAAAAAAAAA 1

//#include "pcm_client.h"



/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

//#define PCM_OP_READ_OBJ	3	/* read list of fields */


PCM_ARRAY_CONN ARRAY_CONN[MAX_CONTEXT_CONN];
int keyConn;

long globalSizeCustomFields;
long globalSizeCustomOps;

/* {{{ void test1() */
PHP_FUNCTION(test1)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("The extension %s is loaded and working!\r\n", "pcm_client");
}
/* }}} */

/* {{{ string test2( [ string $var ] ) */
PHP_FUNCTION(test2)
{
	char *var = "World";
	size_t var_len = sizeof("World") - 1;
	zend_string *retValFieldSpec;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(var, var_len)
	ZEND_PARSE_PARAMETERS_END();

	retValFieldSpec = strpprintf(0, "Hello %s", var);

	RETURN_STR(retValFieldSpec);
}
/* }}}*/




/*
PHP_FUNCTION(get_linked_list_data)
{
    zval result;
    array_init(&result);

    pcm_flist *current = first_node_of_linked_list; // Suponha que first_node_of_linked_list seja o ponteiro para o primeiro n� da sua lista encadeada

    while (current != NULL)
    {
        zval node_data;
        array_init(&node_data);

        add_assoc_string(&node_data, "field", current->field);
        add_assoc_string(&node_data, "type", current->type);
        add_assoc_long(&node_data, "index_array", current->index_array);
        add_assoc_long(&node_data, "size", current->size);
        // Adicione outras propriedades conforme necess�rio

        // Adiciona os dados do n� atual ao array de resultado
        add_next_index_zval(&result, &node_data);

        current = current->next_flist; // Avan�a para o pr�ximo n�
    }

    RETURN_ZVAL(&result, 0, 1);
}
*/







/* {{{ void pin_virtual_time() */
PHP_FUNCTION(pin_virtual_time)
{

	pcm_context_t	*ctxp = NULL;
	pin_errbuf_t	ebuf;
	int64		database;
	time_t		virtual_time;

	char msg[256];

	ZEND_PARSE_PARAMETERS_NONE();

	/***********************************************************
	 * Clear the error buffer (for safety).
	 ***********************************************************/
	PIN_ERR_CLEAR_ERR(&ebuf);

	virtual_time = pin_virtual_time((time_t *)NULL);	
	
	sprintf(msg, __PROGRAM__" pin_virtual_time(): %lld", virtual_time);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg);


	RETURN_LONG(virtual_time);

}
/* }}} */









/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(pcm_client)
{
#if defined(ZTS) && defined(COMPILE_DL_PCM_CLIENT)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(pcm_client)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "pcm_client support", "enabled");
	php_info_print_table_end();
}
/* }}} */






/* {{{ pcm_client_module_entry */
zend_module_entry pcm_client_module_entry = {
	STANDARD_MODULE_HEADER,
	"pcm_client",					/* Extension name */
	ext_functions,					/* zend_function_entry */
	PHP_MINIT(pcm_client),							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(pcm_client),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(pcm_client),			/* PHP_MINFO - Module info */
	PHP_PCM_CLIENT_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PCM_CLIENT
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(pcm_client)
#endif
