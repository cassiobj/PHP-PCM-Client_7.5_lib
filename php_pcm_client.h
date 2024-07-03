/* pcm_client extension for PHP */

#ifndef PHP_PCM_CLIENT_H
# define PHP_PCM_CLIENT_H

extern zend_module_entry pcm_client_module_entry;
# define phpext_pcm_client_ptr &pcm_client_module_entry

# define PHP_PCM_CLIENT_VERSION "0.1.0"

PHP_MINIT_FUNCTION(pcm_client);






// pcm_poid_functions.c
PHP_FUNCTION(pcm_get_poid_db);
PHP_FUNCTION(pcm_get_poid_type);
PHP_FUNCTION(pcm_get_poid_number);
PHP_FUNCTION(pcm_create_poid);

// pcm_transactions.c
PHP_FUNCTION(pcm_open_transaction);
PHP_FUNCTION(pcm_abort_transaction);
PHP_FUNCTION(pcm_commit_transaction);

// pcm_cm_functions.c
PHP_FUNCTION(pcm_op);
PHP_FUNCTION(pcm_set_context);

// pcm_fields_ops_logic.c
PHP_FUNCTION(pcm_get_field_spec);
PHP_FUNCTION(pcm_get_custom_opcode);

// pcm_xml_functions.c
PHP_FUNCTION(pcm_xml_to_flist);

// pcm_pinconf.c
PHP_FUNCTION(pcm_pin_conf);

// pcm_flist.c
PHP_FUNCTION(pcm_flist_set);



# if defined(ZTS) && defined(COMPILE_DL_PCM_CLIENT)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_PCM_CLIENT_H */
