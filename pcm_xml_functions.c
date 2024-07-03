
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



int getXML(xmlNodePtr node, char **result, int *depth);
char *getFieldType(char *fieldName);

/* {{{ void pcm_xml_to_flist() */
PHP_FUNCTION(pcm_xml_to_flist)
{

	pcm_context_t	*ctxp = NULL;
	pin_errbuf_t	ebuf;
	int64		database;
	//char *retorno = NULL;
	char *retorno = NULL;

	char *xml_entrada;
	size_t s_xml_entrada;

	xmlDocPtr doc;
	xmlNodePtr root, node;
	int depth=0;
	char msg[BUFSIZ*16];
	

	ZEND_PARSE_PARAMETERS_START(0, 1)
			Z_PARAM_OPTIONAL
			Z_PARAM_STRING(xml_entrada, s_xml_entrada);
	ZEND_PARSE_PARAMETERS_END();

	doc = xmlReadMemory(xml_entrada, s_xml_entrada, "pcm_client.xml", NULL, 0);

	if (doc == NULL) {
		sprintf(msg, "Error converting XML Content: %s \n",xml_entrada );
		fprintf(stderr, msg );
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
		RETURN_THROWS();
		RETURN_NULL();
	}

	root = xmlDocGetRootElement(doc);

	if (getXML(root, &retorno, &depth) > 0)
	{
		sprintf(msg, __PROGRAM__ " pcm_xml_to_flist(). Error Retrieving XML Content : %s \n", xml_entrada );
		fprintf(stderr, msg );
		xmlFreeDoc(doc);
		
		zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
		RETURN_THROWS();
		RETURN_NULL();
	}

	xmlFreeDoc(doc);



	RETURN_STRING( retorno );
}



/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


int getXML(xmlNodePtr node, char **result, int *depth) {
    xmlNodePtr curNode;

    char line[BUFSIZ*2];
    char content[BUFSIZ*2];
    char field[256];
	char fieldType[256]; 
	char space[32]; 
	int flagSub = 0;
	int i = 0;
	int contArray[64] ;

    // Loop através dos nós filhos do nó atual
	*depth = *depth + 1;

	memset(&contArray, 0, sizeof(contArray));

	memset(space, 0, 32);
	for (i=0; i < (*depth - 2); i++ )
		strcat(space, "  ");
    for (curNode = node; curNode; curNode = curNode->next) {
        // Se o nó atual for um nó de texto, concatene seu conteúdo é string de resultado
    	//fprintf(stderr, "Entrou1.\n");

		if (curNode->type == XML_ELEMENT_NODE &&
			*depth == 1)
		{			
			*depth = *depth + 1;
			sprintf(field, "%s", curNode->name );
			if ( strncmp(field, "PCM_CLIENT",  strlen(field) ) == 1 ) // first node is not PCM_CLIENT key name
			{
				fprintf(stderr, "Error converting XML.\n");
				fprintf(stderr, "XML root is not <PCM_CLIENT>.\n");
				return(1);
			}
			flagSub = 1;
			
		}
		else if (curNode->type == XML_ELEMENT_NODE && *depth > 1)
		{
			memset(line, 0,BUFSIZ*2);
    		memset(content, 0,BUFSIZ*2);
	
			
			sprintf(field, "%s", curNode->name );
	   		//fprintf(stderr, "Entrou f: %s.\n", field);
			memset(fieldType, 0, 256);   		
			strcpy(fieldType, getFieldType(field));
			
			
			if (strcmp(fieldType, "POID") == 0)			
			{
				contArray[*depth] = 0;
    			sprintf(line, "%d%s%s      %s [0] %s 0\n",*depth -3, space, curNode->name, fieldType , xmlNodeGetContent(curNode));
			}
			else if (strcmp(fieldType, "TSTAMP") == 0)
			{
				contArray[*depth] = 0;
    			sprintf(line, "%d%s%s      %s [0] (%s) \n",*depth -3, space, curNode->name, fieldType , xmlNodeGetContent(curNode));
			}
			else if (strcmp(fieldType, "DECIMAL") == 0)
			{
				contArray[*depth] = 0;
    			sprintf(line, "%d%s%s      %s [0] %s \n",*depth -3, space, curNode->name, fieldType , xmlNodeGetContent(curNode));
			}
			else if ( (strcmp(fieldType, "ARRAY") == 0) || (strcmp(fieldType, "SUBSTRUCT") ==0) )
			{					
				flagSub = 1;
				xmlChar *elemValue = xmlGetProp(curNode, BAD_CAST "elem");
				 if (elemValue != NULL)
				 {
					sprintf(line, "%d%s%s      %s [%d]  \n",*depth -3, space, curNode->name, fieldType , (atoi((char*)elemValue) ) );
					contArray[*depth] = atoi((char*)elemValue);	
				 }
				 else
				 {
				 	sprintf(line, "%d%s%s      %s [%d]  \n",*depth -3, space, curNode->name, fieldType , contArray[*depth] );					
				 }
				contArray[*depth]++;
				 
			}
			else if (strcmp(fieldType, "STR") == 0)
			{			
    			sprintf(line, "%d%s%s      %s [0] \"%s\" \n",*depth -3, space, curNode->name, fieldType , xmlNodeGetContent(curNode));
				contArray[*depth] = 0;
			}	


    		if (*result == NULL)
			{
    			*result = (char *) calloc ( (strlen(line) + 1), sizeof(char));
				sprintf(*result, "%s", line);			
			}
			else
			{
				*result = (char *)realloc(*result, ( (strlen(line) + 1)  + strlen(*result) )  * sizeof(char));
				sprintf(*result, "%s%s", *result, line);			
			}
		}

        // Se o no atual tiver filhos, chama a funcao recursivamente para cada filho
        if (curNode->children != NULL && flagSub) {        	        	
            getXML(curNode->children, result, depth);
        }
    }
	*depth = *depth - 1;
    return(0);
}





char *getFieldType(char *fieldName)
{
	char *retorno = NULL;

	pin_flist_t	*s_flistp = NULL;
	pin_flist_t	*fld_flistp = NULL;
	pin_flist_t	*r_flistp = NULL;
	poid_t		*search_poid;
	void		*vp = NULL;

	int 		*fieldType = NULL;

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
		PCM_CONNECT(&(ARRAY_CONN[keyConn]).CTXP,  &(ARRAY_CONN[keyConn]).DATABASE  , &EBUF);
	}
	if( PIN_ERR_IS_ERR( &EBUF ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" getFieldType: error connection", &EBUF);
		fprintf(stderr, "PCM-Client pc_op: error connection\n");
		return( (char *) NULL);
	}
	
	
	s_flistp = PIN_FLIST_CREATE(&EBUF);

	search_poid = PIN_POID_CREATE( ARRAY_CONN[keyConn].DATABASE  , "/dd/fields", (int64)-1, &EBUF);

	PIN_FLIST_FLD_PUT(s_flistp, PIN_FLD_POID, (void *)search_poid, &EBUF);
	
	
	fld_flistp = PIN_FLIST_ELEM_ADD(s_flistp, PIN_FLD_FIELD, 0, &EBUF);	
	PIN_FLIST_FLD_SET(fld_flistp, PIN_FLD_FIELD_NUM, (void *) vp, &EBUF);
	PIN_FLIST_FLD_SET(fld_flistp, PIN_FLD_FIELD_NAME, (char *) fieldName, &EBUF);

	//PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, s_flist );
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" getFieldType flist in: ",s_flistp);


	PCM_OP( ARRAY_CONN[keyConn].CTXP, PCM_OP_SDK_GET_FLD_SPECS, 0, s_flistp, &r_flistp, &EBUF);
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__" getFieldType flist out: ",r_flistp);

	if( PIN_ERR_IS_ERR( &EBUF ))
	{
		PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, __PROGRAM__" getFieldType error executing PCM_OP_SDK_GET_FLD_SPECS", &EBUF);
		return( (char *) NULL);
	}
	

	// gettingg fields
	fld_flistp = PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_FIELD, 0, 0, &EBUF);
	
	if (fld_flistp != NULL)
	{		
		fieldType = PIN_FLIST_FLD_GET(fld_flistp, PIN_FLD_FIELD_TYPE, 1, &EBUF);
		retorno = (char *) calloc ( (strlen(PCM_FIELD_TYPE[*fieldType]) + 1), sizeof(char));
		sprintf(retorno, "%s", PCM_FIELD_TYPE[*fieldType]);
	}

	PIN_FLIST_DESTROY_EX(&s_flistp, NULL);
	PIN_FLIST_DESTROY_EX(&r_flistp, NULL);
	
	return ((char *) retorno);


}
