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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pinlog.h"
#include "pin_errs.h"
//#include "pcm.h"
#include "ops/base.h"
#include "pin_cust.h"
//#include "pcm_client.h"


#include "pcm_fields_ops_logic.h"
#include "pcm_structures.h"

PCM_ARRAY_CONN ARRAY_CONN[MAX_CONTEXT_CONN];
int keyConn;
char        *CUSTOM_FLD_OP_TABLE;
pcm_opcode  *opcodeList;

long globalSizeCustomFields;
long globalSizeCustomOps;






/* {{{ void pcm_get_field_spec() */
PHP_FUNCTION(pcm_get_field_spec) {	
    // Cria um array associativo para armazenar os dados
    zval retValFieldSpec;
    array_init(&retValFieldSpec);

	char *field_str;
	size_t s_field_str;
	int   flag_alloc = 0;

	field_spec *fields = NULL;
	
    char msg[BUFSIZ];


	ZEND_PARSE_PARAMETERS_START(0, 1)
			Z_PARAM_OPTIONAL
			Z_PARAM_STRING(field_str, s_field_str);
	ZEND_PARSE_PARAMETERS_END();

	
	
	fields = (field_spec *) malloc(sizeof(field_spec));
	fields->name=NULL;
	fields->descr=NULL;
	fields->type=NULL;
	if (pcm_array_find_custom_field( field_str, &CUSTOM_FLD_OP_TABLE, &fields) == -1)
	{	
		if (getFieldSpec(field_str, &fields) == -1)
		{
			PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" pcm_get_field_spec: getting informations", &EBUF);            
			RETURN_NULL();
		
		}		
	}
	   

	if (fields->type != NULL)
	{
		// Adiciona dados ao array
		
		add_assoc_string(&retValFieldSpec, "type", fields->type );
		if (fields->descr != NULL)
			add_assoc_string(&retValFieldSpec, "descr", fields->descr );
		else
			add_assoc_string(&retValFieldSpec, "descr", "" );

		add_assoc_long(&retValFieldSpec, "number", fields->number);
		add_assoc_long(&retValFieldSpec, "type_number", fields->numberType);
		add_assoc_long(&retValFieldSpec, "status", fields->status);
	}
	else
		RETURN_NULL();


	if (fields != NULL && fields->name != NULL)
		free(fields->name);
	if (fields != NULL && fields->descr != NULL)
		free(fields->descr);
	if (fields != NULL && fields->type != NULL)
		free(fields->type);

	free(fields);
	
    
    // Retorna o array associativo
    RETURN_ZVAL(&retValFieldSpec, 0, 1);
}
/* }}} */




/* {{{ void pcm_get_custom_opcode() */
PHP_FUNCTION(pcm_get_custom_opcode) {	
    // Cria um array associativo para armazenar os dados
    zval retValFieldSpec;
    array_init(&retValFieldSpec);

	char *opcode_str;
	size_t s_opcode_str;
	int   flag_alloc = 0;

	pcm_opcode *retorno;
	

	ZEND_PARSE_PARAMETERS_START(0, 1)
			Z_PARAM_OPTIONAL
			Z_PARAM_STRING(opcode_str, s_opcode_str);
	ZEND_PARSE_PARAMETERS_END();

	/*for (i = 0; i< globalSizeCustomOps; i++)
	{
		fprintf(stderr, "|%s|\n", (opcodeList)[i].opcode );
	}*/

	retorno = pcm_opcode_search(opcodeList, globalSizeCustomOps, opcode_str);
	if (retorno==NULL)
	{        
		RETURN_LONG(-1);
	}
	RETURN_LONG(retorno->number);
	
}	


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/







long  int pcm_array_find_custom_field( char *key, char **tabela,  field_spec **fields)
{
    long left = 0;
    long right = globalSizeCustomFields - 1;
    char aux[BUFSIZ];
    field_spec *prov;
    long int index =0 ;

    



    sprintf(aux, "%-60.60s", key);    
    if ( (index = findReg(*tabela, aux, 0, globalSizeCustomFields, PCM_SIZE_LINE_CUSTOM_FIELD, 60 )  ) >= 0)
    {
        int indReg = 0;

        // getting register type (last field)
        sprintf(aux, "%-1.1s", *tabela + index + (PCM_SIZE_LINE_CUSTOM_FIELD-2));        
        (*fields)->typeInformaton = aux[0];

        // getting name
        sprintf(aux, "%-60.60s", *tabela + index);
        rtrim(aux);
        (*fields)->name = (char *) calloc ( (strlen(aux) + 1), sizeof(char));
        sprintf((*fields)->name, "%s", aux);
        indReg +=61;

        

        sprintf(aux, "%-9.9s", *tabela + index + indReg);
        indReg+=10;

        if ((*fields)->typeInformaton == 'F')
        {
            rtrim(aux);
            (*fields)->type = (char *) calloc ( (strlen(aux) + 1), sizeof(char));
            memset((*fields)->type, 0, (strlen(aux) + 1));
            strcpy((*fields)->type, aux);

            (*fields)->numberType = -1;
            if (strcmp("UNUSED", aux) == 0)
                (*fields)->numberType = 0;
            if (strcmp("INT", aux) == 0)
                (*fields)->numberType = 1;
            if (strcmp("UINT", aux) == 0)
                (*fields)->numberType = 2;
            if (strcmp("ENUM", aux) == 0)
                (*fields)->numberType = 3;
            if (strcmp("NUM", aux) == 0)
                (*fields)->numberType = 4;
            if (strcmp("STR", aux) == 0)
                (*fields)->numberType = 5;
            if (strcmp("BUF", aux) == 0)
                (*fields)->numberType = 6;
            if (strcmp("POID", aux) == 0)
                (*fields)->numberType = 7;
            if (strcmp("TSTAMP", aux) == 0)
                (*fields)->numberType = 8;
            if (strcmp("ARRAY", aux) == 0)
                (*fields)->numberType = 9;
            if (strcmp("SUBSTRUCT", aux) == 0)
                (*fields)->numberType = 10;
            if (strcmp("OBJ", aux) == 0)
                (*fields)->numberType = 11;
            if (strcmp("BINSTR", aux) == 0)
                (*fields)->numberType = 12;
            if (strcmp("ERR", aux) == 0)
                (*fields)->numberType = 13;
            if (strcmp("DECIMAL", aux) == 0)
                (*fields)->numberType = 14;
            if (strcmp("TIME", aux) == 0)
                (*fields)->numberType = 15;
            if (strcmp("TEXTBUF", aux) == 0)
                (*fields)->numberType = 16;
            if (strcmp("ERRBUF", aux) == 0)
                (*fields)->numberType = 13;
        }    

        sprintf(aux, "%-6.6s", *tabela + index + indReg);
        indReg+=7;
        rtrim(aux);
        (*fields)->number = atol(aux);

        (*fields)->status = 3;
        (*fields)->descr=NULL;
        return index;
    } 

    return (-1); // Registro não encontrado
}




//long pcm_load_and_sort_field_spec(char **tabela, pcm_opcode **opcodeList, int *totField, int *totOpcode)
long pcm_load_and_sort_field_spec(  )
{
	int i =0;
	long size = 0;
    long tam = 0;
	FILE *file = NULL;
	char msg[BUFSIZ];
    char aux[BUFSIZ];
    char *PosI, *PosF;
    char *filename;
    
    
    globalSizeCustomFields = 0;
    globalSizeCustomOps = 0;


    if (pcm_get_string_pin_conf(__PROGRAM__, "php_ops_fields_extension_file", &filename ) == -1)
    {
        sprintf(msg, __PROGRAM__" Error getting information custom fields and opcodes file from pin.conf  - %s  php_ops_fields_extension_file\n", __PROGRAM__);
    	//fprintf(stderr, msg);
		//pin_set_err(&EBUF, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, (&EBUF)->pin_err, 0, 0, 0);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_WARNING, msg , &EBUF);
		return -1;        
    }
    PIN_ERR_CLEAR_ERR(&EBUF); 

    file = fopen(filename, "r");
    if (file == NULL) {
    	sprintf(msg, __PROGRAM__" Error opening file %s: %s\n", filename, strerror(errno));
    	fprintf(stderr, msg);
		pin_set_err(&EBUF, PIN_ERRLOC_UTILS, PIN_ERRCLASS_APPLICATION, errno, 0, 0, 0);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);
		return -1;
	}

    size = floor(pcm_get_file_size(file)/(PCM_SIZE_LINE_CUSTOM_FIELD + 1)); //+1 bcz \n
    tam  = sizeof(char) * size * PCM_SIZE_LINE_CUSTOM_FIELD ;

    CUSTOM_FLD_OP_TABLE = (char *) malloc( tam + 1 );    
    memset(CUSTOM_FLD_OP_TABLE, 0, (tam + 1) );    
    
    
    

    // Carrega os dados do arquivo CSV para a array
    while (1) {
        char line[BUFSIZ];    
        if (fgets(line, sizeof(line), file) != NULL)
            if (line[PCM_SIZE_LINE_CUSTOM_FIELD-2]=='F')
            {
                globalSizeCustomFields ++;
                strncat(CUSTOM_FLD_OP_TABLE, line, PCM_SIZE_LINE_CUSTOM_FIELD);
            }
            else
            {
                if (globalSizeCustomOps == 0)
                    opcodeList = (pcm_opcode *) malloc (sizeof(pcm_opcode));
                else
                    opcodeList  = (pcm_opcode *) realloc(opcodeList, (globalSizeCustomOps+1) * sizeof(pcm_opcode));

                //fprintf(stderr, "passou\n");
                memset(aux, 0, BUFSIZ);
                sprintf((opcodeList)[globalSizeCustomOps].opcode, "%-60.60s", line);
                rtrim((opcodeList)[globalSizeCustomOps].opcode);
                //fprintf(stderr, "%s\n", (*opcodeList)[globalSizeCustomOps].opcode);

                sprintf(aux, "%-6.6s", line + 71);
                (opcodeList)[globalSizeCustomOps].number = atol(aux);
                
                globalSizeCustomOps++;
                //fprintf(stderr, "fim\n");        
            }
        else
            break;

        
    }
    
    fclose(file);

    return 0;
}

// Função de comparação para a busca binária
int compare_opcodes(const void *a, const void *b) {
    return strcmp(((pcm_opcode*)a)->opcode, ((pcm_opcode*)b)->opcode);
}

// Função de busca binária
pcm_opcode *pcm_opcode_search(pcm_opcode *array, int size, const char *key) 
{
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = strcmp(array[mid].opcode, key);

        
        if (cmp == 0) {
            return &array[mid];
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return NULL; // Elemento não encontrado
}





int getFieldSpec(char *fieldName, field_spec **fields)
{
	char *retorno = NULL;

	pin_flist_t	*s_flistp = NULL;
	pin_flist_t	*fld_flistp = NULL;
	pin_flist_t	*r_flistp = NULL;
	poid_t		*search_poid;
	void		*vp = NULL;

	int 		*fieldType = NULL;
	int 		*fieldNumber = NULL;
	int 		*status = NULL;
	char		*descr = NULL;
	

	/*
r << XX 3
0 PIN_FLD_POID           POID [0] 0.0.0.1 /dd/fields -1 0
0 PIN_FLD_FIELD           ARRAY [0] allocated 20, used 6
1     PIN_FLD_FIELD_NUM         ENUM [0] 0
1     PIN_FLD_FIELD_NAME            STR [0] "PIN_FLD_DUE_T"
XX
xop PCM_OP_SDK_GET_FLD_SPECS 0 3
*/

	
	
	PIN_ERR_CLEAR_ERR(&EBUF);
	
	//PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "PCM-Client pc_op()");

	if (ARRAY_CONN[keyConn].CTXP == NULL)
	{
		pcm_connect_cm( &(ARRAY_CONN[keyConn]).DATABASE );
		//PCM_CONNECT(&CTXP, &DATABASE, &EBUF);
	}
	if( PIN_ERR_IS_ERR( &EBUF ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" getFieldSpec: error connection", &EBUF);
		fprintf(stderr, __PROGRAM__" pc_op: error connection\n");
		return( -1);
	}
	
	
	s_flistp = PIN_FLIST_CREATE(&EBUF);

	search_poid = PIN_POID_CREATE(  ARRAY_CONN[keyConn].DATABASE  , "/dd/fields", (int64)-1, &EBUF);

	PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, (void *)search_poid, &EBUF);
	
	
	fld_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_FIELD, 0, &EBUF);	
	PIN_FLIST_FLD_SET(fld_flistp, PIN_FLD_FIELD_NUM, (void *) vp, &EBUF);
	PIN_FLIST_FLD_SET(fld_flistp, PIN_FLD_FIELD_NAME, (char *) fieldName, &EBUF);

	//PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, s_flist );
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" getFieldSpec flist in: ",s_flistp);


	PCM_OP( ARRAY_CONN[keyConn].CTXP , PCM_OP_SDK_GET_FLD_SPECS, 0, s_flistp, &r_flistp, &EBUF);
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" getFieldSpec flist out: ",r_flistp);

	if( PIN_ERR_IS_ERR( &EBUF ))
	{
        PIN_FLIST_DESTROY_EX(&s_flistp, NULL);
	    PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" getFieldSpec error executing PCM_OP_SDK_GET_FLD_SPECS", &EBUF);
		return(-1);
	}
	

	// gettingg fields
	fld_flistp = PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_FIELD, 0, 0, &EBUF);
	
	if (fld_flistp != NULL)
	{		
		
		fieldType = PIN_FLIST_FLD_GET(fld_flistp, PIN_FLD_FIELD_TYPE, 1, &EBUF);
		(*fields)->type = (char *) calloc ( (strlen(PCM_FIELD_TYPE[*fieldType]) + 1), sizeof(char));
		//retorno = (char *) calloc ( (strlen(PCM_FIELD_TYPE[*fieldType]) + 1), sizeof(char));
		sprintf((*fields)->type, "%s", PCM_FIELD_TYPE[*fieldType]);
		(*fields)->numberType = *fieldType;

		(*fields)->number = 0;
		fieldNumber = PIN_FLIST_FLD_GET(fld_flistp, PIN_FLD_FIELD_NUM, 1, &EBUF);
		if (fieldNumber!=NULL)
			(*fields)->number = *fieldNumber;

		(*fields)->status = 0;
		status = PIN_FLIST_FLD_GET(fld_flistp, PIN_FLD_STATUS, 1, &EBUF);
		if (status!=NULL)
			(*fields)->status = *status;

		(*fields)->descr = NULL;
		descr = PIN_FLIST_FLD_GET(fld_flistp, PIN_FLD_DESCR, 1, &EBUF);
		if (descr!=NULL)
		{
			(*fields)->descr = (char *) calloc ( (strlen(descr) + 1), sizeof(char));
			//retorno = (char *) calloc ( (strlen(PCM_FIELD_TYPE[*fieldType]) + 1), sizeof(char));
			sprintf((*fields)->descr, "%s", descr);
		}
		(*fields)->name = NULL;
		
	}
	else
	{
		(*fields)->name = NULL;
		(*fields)->type = NULL;
		(*fields)->descr = NULL;
	}

	PIN_FLIST_DESTROY_EX(&s_flistp, NULL);
	PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
	
	return ( 0);


}