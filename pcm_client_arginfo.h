/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: efdd79c2c8ccff694699c86fdd6248a13839c744 */

//#define str "str"

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_test1, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_test2, 0, 0, IS_STRING, 0)
	ZEND_ARG_INFO(0, str)
	//ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, str, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

/*
ZEND_BEGIN_ARG_INFO(arginfo_pcm_connect, 0)
    ZEND_ARG_INFO(0, x)
ZEND_END_ARG_INFO()
*/

ZEND_BEGIN_ARG_INFO(arginfo_pcm_op, 0)
    ZEND_ARG_INFO(0, opcode)
	ZEND_ARG_INFO(0, flist)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcm_xml_to_flist, 0, 0, IS_STRING, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pin_virtual_time, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcm_get_poid_number, 0, 0, IS_LONG, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcm_get_poid_type, 0, 0, IS_STRING, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcm_get_poid_db, 0, 0, IS_LONG, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcm_create_poid, 0, 0, IS_STRING, 0)
    ZEND_ARG_INFO(0, database)
	ZEND_ARG_INFO(0, type)
	ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_pcm_get_field_spec, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pcm_get_custom_opcode, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pcm_pin_conf, 0, 0, 1)
	ZEND_ARG_INFO(0, program)
	ZEND_ARG_INFO(0, token)
	ZEND_ARG_INFO(0, val_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_pcm_open_transaction, 0, 0, 1)
	ZEND_ARG_INFO(0, pcm_op_flag)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcm_commit_transaction, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcm_abort_transaction, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcm_set_context, 0, 0, IS_LONG, 0)
	ZEND_ARG_INFO(0, contextNumber)
	ZEND_ARG_INFO(0, database)
ZEND_END_ARG_INFO()



ZEND_BEGIN_ARG_INFO_EX(arginfo_pcm_flist_set, 0, 0, 1)
	ZEND_ARG_INFO(0, flist)
	ZEND_ARG_INFO(0, field)
	ZEND_ARG_INFO(0, element)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(test1);
ZEND_FUNCTION(test2);
//ZEND_FUNCTION(pcm_connect);

ZEND_FUNCTION(pcm_op);
ZEND_FUNCTION(pin_virtual_time);

ZEND_FUNCTION(pcm_xml_to_flist);
ZEND_FUNCTION(pcm_get_poid_number);
ZEND_FUNCTION(pcm_get_poid_type);
ZEND_FUNCTION(pcm_get_poid_db);
ZEND_FUNCTION(pcm_create_poid);
ZEND_FUNCTION(pcm_get_field_spec);
ZEND_FUNCTION(pcm_get_custom_opcode);
ZEND_FUNCTION(pcm_open_transaction);
ZEND_FUNCTION(pcm_commit_transaction);
ZEND_FUNCTION(pcm_abort_transaction);
ZEND_FUNCTION(pcm_set_context);
ZEND_FUNCTION(pcm_flist_set);
ZEND_FUNCTION(pcm_pin_conf);



static const zend_function_entry ext_functions[] = {
	ZEND_FE(test1, arginfo_test1)
	ZEND_FE(test2, arginfo_test2)	
	ZEND_FE(pcm_op, arginfo_pcm_op)
	ZEND_FE(pcm_xml_to_flist, arginfo_pcm_xml_to_flist)
	ZEND_FE(pin_virtual_time, arginfo_pin_virtual_time)
	ZEND_FE(pcm_get_poid_number, arginfo_pcm_get_poid_number)
	ZEND_FE(pcm_get_poid_type, arginfo_pcm_get_poid_type)
	ZEND_FE(pcm_get_poid_db, arginfo_pcm_get_poid_db)
	ZEND_FE(pcm_create_poid, arginfo_pcm_create_poid)
	ZEND_FE(pcm_get_field_spec, arginfo_pcm_get_field_spec)
	ZEND_FE(pcm_get_custom_opcode, arginfo_pcm_get_custom_opcode)	
	ZEND_FE(pcm_open_transaction, arginfo_pcm_open_transaction)	
	ZEND_FE(pcm_commit_transaction, arginfo_pcm_commit_transaction)	
	ZEND_FE(pcm_abort_transaction, arginfo_pcm_abort_transaction)	
	ZEND_FE(pcm_set_context, arginfo_pcm_set_context)	
	ZEND_FE(pcm_pin_conf, arginfo_pcm_pin_conf)
	ZEND_FE(pcm_flist_set, arginfo_pcm_flist_set)	
	ZEND_FE_END
};






