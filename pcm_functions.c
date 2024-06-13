/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PHP-PCM-Client-lib
  PHP Extension interact with Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */


#include <stdio.h>
#include "pinlog.h"
#include "pin_errs.h"
#include "pcm.h"
#include "ops/base.h"
#include "pin_cust.h"
//#include "pcm_client.h"



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


#include <libxml/parser.h>
#include <libxml/tree.h>

#include "pcm_structures.h"
#include "pcm_defines.h"


PCM_ARRAY_CONN ARRAY_CONN[MAX_CONTEXT_CONN];
int keyConn;

void convert_array_flist(pin_flist_t **s_flistp, single_flist *flist_tree);

void convert_array_flist(pin_flist_t **s_flistp, single_flist *flist_tree)
{
  char msg[BUFSIZ];

  if( PIN_ERR_IS_ERR( &EBUF ))
	{
    
      sprintf(msg, __PROGRAM__ " pcm_op: error converting flist "  );
      PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg, &EBUF);		      
      zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
      return;
	}


    //fprintf(stderr, "funçao Number %s - %ld \n", flist_tree->field, flist_tree->fieldNumber );
 		while (flist_tree!= NULL)
		{      
		
			//  fprintf(stderr, "Field Number %s - %ld ", flist_tree->field, flist_tree->fieldNumber );
			 // fprintf(stderr, "Field type %s \n", flist_tree->type );        


        if (strcmp(flist_tree->type, "POID")==0)
        {
            poid_t   *auxPoid = NULL;
            
            sprintf(msg, __PROGRAM__ " pcm_op: poid to convert: %s", (char *)flist_tree->value );
            PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg );

            auxPoid = PIN_POID_FROM_STR((char *)flist_tree->value, NULL , &EBUF)	;

            if( PIN_ERR_IS_ERR( &EBUF ))
            {     
                  sprintf(msg, __PROGRAM__ " pcm_op: error converting poid: %s "  , (char *)flist_tree->value );
                  PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg, &EBUF);
                  zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
                  return;
            }            
                        
            PIN_FLIST_FLD_SET(*s_flistp, PIN_MAKE_FLD(PIN_FLDT_POID, flist_tree->fieldNumber) , (poid_t *) auxPoid , &EBUF);          
            
        }
        else if (strcmp(flist_tree->type, "STR")==0)
        {
            
            sprintf(msg, __PROGRAM__ " pcm_op: str to flist: %s", (char *)flist_tree->value );
            PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg );
            PIN_FLIST_FLD_SET(*s_flistp, PIN_MAKE_FLD(PIN_FLDT_STR, flist_tree->fieldNumber) , (char *)flist_tree->value, &EBUF);          
        }
        else if (strcmp(flist_tree->type, "INT") == 0)
        {
            int32 *auxInt = (int32 *) flist_tree->value;

            sprintf(msg, __PROGRAM__ " pcm_op: INT to flist: %d", *auxInt );
            PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg );

            
            PIN_FLIST_FLD_SET(*s_flistp, PIN_MAKE_FLD(PIN_FLDT_INT, flist_tree->fieldNumber), (int32 *) auxInt, &EBUF);
        }
        else if (  strcmp(flist_tree->type, "ENUM") == 0  )         
        {          
                int32 *auxInt = (int32 *) flist_tree->value;

                sprintf(msg, __PROGRAM__ " pcm_op: ENUM to flist: %d", *auxInt );
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg );

                
                PIN_FLIST_FLD_SET(*s_flistp, PIN_MAKE_FLD(PIN_FLDT_ENUM, flist_tree->fieldNumber), (int32 *) auxInt, &EBUF);      
        }
        else if (  strcmp(flist_tree->type, "NUM") == 0 )         
        {          
                int32 *auxInt = (int32 *) flist_tree->value;

                sprintf(msg, __PROGRAM__ " pcm_op: NUM to flist: %d", *auxInt );
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg );

                
                PIN_FLIST_FLD_SET(*s_flistp, PIN_MAKE_FLD(PIN_FLDT_NUM, flist_tree->fieldNumber), (int32 *) auxInt, &EBUF);
        }
        else if ( strcmp(flist_tree->type, "TSTAMP") == 0 )         
        {          
              int32 *auxInt = (int32 *) flist_tree->value;

              sprintf(msg, __PROGRAM__ " pcm_op: INT to flist: %d", *auxInt );
              PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg );

              
              PIN_FLIST_FLD_SET(*s_flistp, PIN_MAKE_FLD(PIN_FLDT_TSTAMP, flist_tree->fieldNumber), (int32 *) auxInt, &EBUF);

        }
        else if ( strcmp(flist_tree->type, "DECIMAL") == 0 )
        {          
              char aux[128];
              long double *auxDouble =  (long double *) flist_tree->value;
              pin_decimal_t *value  = NULL;

              sprintf(aux, "%.4Lf", *auxDouble  ) ;

              sprintf(msg, __PROGRAM__ " pcm_op: DECIMAL to flist: %s", aux );
              PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg );

              value = pbo_decimal_from_str(aux , &EBUF);
              //value = pbo_decimal_from_double_round( *auxDouble , &EBUF);
              
              PIN_FLIST_FLD_SET(*s_flistp, PIN_MAKE_FLD( PIN_FLDT_DECIMAL, flist_tree->fieldNumber), value , &EBUF);


              pbo_decimal_destroy(&value);
        }
        else if ( strcmp(flist_tree->type, "ARRAY") == 0 ||  strcmp(flist_tree->type, "SUBSTRUCT") == 0 )
        {          
              char aux[128];
              
              /*fprintf(stderr, "Array \n" );
              if (flist_tree->array != NULL)
                fprintf(stderr, "Tem Array TB \n" );
              */

              if (flist_tree->array != NULL)
              {
                  single_flist *flist_tree_array = flist_tree->array;
                  pin_flist_t       *arr_flistp = NULL;
                  
                  arr_flistp = PIN_FLIST_CREATE(&EBUF);

                  // fprintf(stderr, "Chamou criação de flist de array  %s - %ld \n", flist_tree_array->field, flist_tree_array->fieldNumber );

                  convert_array_flist(&arr_flistp,flist_tree->array);

                  if ( strcmp(flist_tree->type, "ARRAY") == 0)
                    PIN_FLIST_FLD_SET(*s_flistp, PIN_MAKE_FLD( PIN_FLDT_ARRAY, flist_tree->fieldNumber), arr_flistp , &EBUF)
                  else
                    PIN_FLIST_FLD_SET(*s_flistp, PIN_MAKE_FLD( PIN_FLDT_SUBSTRUCT, flist_tree->fieldNumber), arr_flistp , &EBUF);
              }
              
              //sprintf(msg, __PROGRAM__ " pcm_op: DECIMAL to flist: %s", aux );
              //PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg );

              //value = pbo_decimal_from_str(aux , &EBUF);
              //value = pbo_decimal_from_double_round( *auxDouble , &EBUF);
              
              //PIN_FLIST_FLD_SET(*s_flistp, PIN_MAKE_FLD(PIN_FLDT_DECIMAL, flist_tree->fieldNumber), value , &EBUF);


              
        }
        else if (strlen(flist_tree->type) > 0)
        {
              sprintf(msg, __PROGRAM__ " pcm_op: Type %s cannot be converted. Use flist string format", flist_tree->type );
              PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_ERROR, msg );
              zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
        }
        
        if( PIN_ERR_IS_ERR( &EBUF ))
        {     
              sprintf(msg, __PROGRAM__ " pcm_op: setting flist data "  );
              PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg, &EBUF);
              zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);
              return;
        }

        
        // PIN_FLIST_PRINT(*s_flistp, stdout, &EBUF);

        
        if (flist_tree->next == NULL)
          break;
        flist_tree = flist_tree->next;
      

        
			
		}

    PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, __PROGRAM__ "pcm_op() flist generated by convert_array_flist", *s_flistp); 

}

void process_flist_array(zval *array, single_flist **flist_tree, int nivel) 
{
    zend_string *key;
    zval *value;

    long int key_field;
    field_spec *fields = NULL;
    char msg[BUFSIZ];

    int type = 0;
    single_flist *new_flist_tree;
    
    
    
    
    
    
    fields = (field_spec *) malloc(sizeof(field_spec));
	  fields->name=NULL;
	  fields->descr=NULL;
	  fields->type=NULL;

    
    // *flistp = PIN_FLIST_CREATE(&EBUF);

    // php_var_dump(array, 1);
    //fprintf(stderr, "Chamou função | ");
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(array), key, value) 
    {

        //*flist_tree = (single_flist *) malloc (sizeof(single_flist));
        type = Z_TYPE_P(value);        
                
        if (key) 
        {
           //fprintf(stderr, "Tem Chave | "); 
           if (*flist_tree == NULL)        
           {
              //fprintf(stderr, "alocou novo \n\n");
              *flist_tree = (single_flist *) malloc (sizeof(single_flist));
              memset(*flist_tree, 0, sizeof(single_flist) );
              (*flist_tree)->next = NULL;
              (*flist_tree)->array = NULL;
              (*flist_tree)->head = *flist_tree;
           }        
           else
           {          
                //fprintf(stderr, "alocou next\n\n");
                // alocating new element          
                new_flist_tree = (single_flist *) malloc (sizeof(single_flist));
                memset(new_flist_tree, 0, sizeof(single_flist) );
                new_flist_tree->head = (*flist_tree)->head;          
                (*flist_tree)->next = (struct single_flist *) new_flist_tree;          
                *flist_tree =  (*flist_tree)->next;          
                (*flist_tree)->next = NULL;
                (*flist_tree)->array = NULL;
            
           } 
           

           // fprintf(stderr, "entrou 1 \n");


            // Key is aarray
            if (type == IS_ARRAY) {
                //fprintf(stderr, "Achou Nivel %d Field key: %s - %ld - [ARRAY] \n", nivel, ZSTR_VAL(key),  fields->number);

                const char *field_name = ZSTR_VAL(key);
                single_flist *nested_flist_tree = NULL;;
                zval value_new;


                if (pcm_array_find_custom_field( field_name, &CUSTOM_FLD_OP_TABLE, &fields) == -1)
                {	
                  if (getFieldSpec(field_name, &fields) == -1)
                  {                
                    sprintf(msg, __PROGRAM__" pcm_op - process_flist_array field not found %s \n", key);
                    fprintf(stderr, msg);		
                    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);		
                    zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);                
                    return ;
                  }		
                }
                //fprintf(stderr, "Achou Field key: %s - %ld \n", field_name,  fields->number);
                key_field = fields->number;
                

                
                array_init(&value_new);

                zend_assign_to_variable(&value_new, value,  IS_ARRAY, true);
                // fprintf(stderr, "entrou 4 \n");

                (*flist_tree)->tab = nivel;
                (*flist_tree)->fieldNumber = key_field;
                
                
                sprintf( (*flist_tree)->type , "%s",  fields->type );
                sprintf( (*flist_tree)->field , "%s",  ZSTR_VAL(key));

                
                nivel++;  
                
                process_flist_array( &value_new, &(*flist_tree)->array, nivel);



                // (*flist_tree)->array = (struct single_flist *) nested_flist_tree;

            } else {

                 const char *field_name = ZSTR_VAL(key);

                if (pcm_array_find_custom_field( field_name, &CUSTOM_FLD_OP_TABLE, &fields) == -1)
                {	
                  if (getFieldSpec(field_name, &fields) == -1)
                  {                
                    sprintf(msg, __PROGRAM__" pcm_op - process_flist_array field not found %s \n", key);
                    fprintf(stderr, msg);		
                    PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR, msg , &EBUF);		
                    zend_throw_exception(pcm_exception_ce, msg , (&EBUF)->pin_err);                
                    return ;
                  }		
                }
                // fprintf(stderr, "Achou Field key: %s - %ld \n", field_name,  fields->number);
                key_field = fields->number;


                

                // Handle other types, such as strings, longs, etc.
                if (type == IS_STRING) 
                {   
                    const char *field_value = Z_STRVAL_P(value);
                    if ( fields->numberType == PIN_FLDT_POID)
                    {                        

                        (*flist_tree)->tab = nivel;
                        (*flist_tree)->fieldNumber = key_field;
                        sprintf( (*flist_tree)->field , "%s", field_name);

                        sprintf( (*flist_tree)->type , "%s", fields->type);

                        (*flist_tree)->value = (char *) malloc (strlen (field_value) + 1);
                        sprintf( (*flist_tree)->value , "%s", field_value);

                        sprintf(msg, __PROGRAM__ ": poid  %s \n", (*flist_tree)->value);
                        PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg);		
                        //fprintf(stderr, msg);

                        
                    }
                    else if  ( fields->numberType == PIN_FLDT_STR )
                    {
                        //PIN_FLIST_FLD_PUT(*flistp,  key_field , (char *) field_value, &EBUF);

                        (*flist_tree)->tab = nivel;
                        (*flist_tree)->fieldNumber = key_field;
                        sprintf( (*flist_tree)->field , "%s", field_name);

                        sprintf( (*flist_tree)->type , "%s", fields->type);

                        (*flist_tree)->value = (char *) malloc (strlen (field_value) + 1);
                        sprintf( (*flist_tree)->value , "%s", field_value);

                        sprintf(msg, __PROGRAM__ ": string  %s \n", (*flist_tree)->value);
                        PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg);		
                        // fprintf(stderr, msg);
                        
                    }                    
                    
                }
                else if (type == IS_LONG) 
                {
                    
                    int64 field_value = Z_LVAL_P(value);
                    
                    (*flist_tree)->tab = nivel;
                    (*flist_tree)->fieldNumber = key_field;
                    sprintf( (*flist_tree)->field , "%s", field_name);

                    sprintf( (*flist_tree)->type , "%s", fields->type);

                    (*flist_tree)->value = (int64 *) malloc (sizeof(int64) );
                    *(int64 *)(*flist_tree)->value = (int64 ) field_value;

                    sprintf(msg, __PROGRAM__ ": long  %lld \n",  *(int64 *)(*flist_tree)->value );
                    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg);		
                    // fprintf(stderr, msg);
              
                }                
                else if (type == IS_DOUBLE) 
                {
                    
                    long double field_value = Z_DVAL_P(value);
                    
                    (*flist_tree)->tab = nivel;
                    (*flist_tree)->fieldNumber = key_field;
                    sprintf( (*flist_tree)->field , "%s", field_name);

                    sprintf( (*flist_tree)->type , "%s", fields->type);

                    (*flist_tree)->value = (long double *) malloc (sizeof(long double) );
                    *(long double *)(*flist_tree)->value = (long double ) field_value;

                    sprintf(msg, __PROGRAM__ ": double  %Lf\n",  *(long double *)(*flist_tree)->value );
                    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, msg);		
                    // fprintf(stderr, msg);
              
                }                
                // Handle other possible types (double, boolean, etc.) as needed
            }
        }
        else
        {
            // fprintf(stderr, "Nao tem Chave | "); 
            if (type == IS_ARRAY) {
                // fprintf(stderr, "Achou Array \n");

                
                zval value_new;

                array_init(&value_new);

                zend_assign_to_variable(&value_new, value,  IS_ARRAY, true);
                // fprintf(stderr, "entrou 4 \n");
                
                /*if (*flist_tree == NULL)        
                {
                    fprintf(stderr, "alocou novo array \n");
                    *flist_tree = (single_flist *) malloc (sizeof(single_flist));
                    memset(*flist_tree, 0, sizeof(single_flist) );
                    (*flist_tree)->next = NULL;
                    (*flist_tree)->array = NULL;
                    (*flist_tree)->head = *flist_tree;
                } */       
                
                nivel++;
                process_flist_array( &value_new, flist_tree , nivel);

                

            }

        }
    } ZEND_HASH_FOREACH_END();

    // PIN_FLIST_PRINT(*flistp, stdout, &EBUF);

    if (fields != NULL && fields->name != NULL)
  		free(fields->name);
    if (fields != NULL && fields->descr != NULL)
      free(fields->descr);
    if (fields != NULL && fields->type != NULL)
      free(fields->type);

    free(fields);

    *flist_tree = (*flist_tree)->head;

    // fprintf(stderr, "Saiu Funcao \n"); 
    
}


void ltrim( char *s )
{
    int i = 0;
    int j = 0;
    char *p;

    while( (' ' == s[i] || '\t' == s[i]) && s[i] )
        i++;

    j = strlen( s+i );
    p =  (char *) malloc(j+1);

    //strncpy( s, s+i, j );
    strncpy( p, s+i, j );
    p[j] = '\0';
    //strcpy(s, p);
    s=p;
    //s[j] = '\0';
}

void rtrim( char *s )
{
    int i = 0, t=0;

    i= t = strlen( s );


    while( (' ' == s[i-1] || '\t' == s[i-1]) && (i-1) > 0 )
        i--;

    if (t!=i) s[i] = '\0';
}

void alltrim( char *s )
{
    rtrim( s );
    ltrim( s );
}



long pcm_get_field(void)
{
	char		logfile[256];
	char		*c_ptr = (char *)NULL;
	char		*program = "PHP-PCM-Client";
	int32		err;

	//c_ptr = PIN_FIELD_GET_NAME(PIN_FLD_POID);

	return (long)PIN_FLD_POID;
}








long pcm_get_file_size(FILE *file) {
    // Salva a posição atual do cursor no arquivo
    long current_position = ftell(file);

    // Move o cursor para o final do arquivo
    fseek(file, 0, SEEK_END);

    // Obtém a posição atual do cursor, que é o tamanho do arquivo
    long size = ftell(file);

    // Restaura a posição do cursor para onde estava originalmente
    fseek(file, current_position, SEEK_SET);

    return size;
}



long int findReg(char *source, char *item,unsigned long start,long end, int regLen, int itemLen)
{
  unsigned long mid=0;
  long int positionIni,positionEnd,positionMid;
  char t1[256+1],t2[256+1],t3[256+1];

  mid = ((start+end) %2 == 0) ? (start+end)/2 : (start+end-1)/2;

  positionIni=start*regLen;
  positionEnd=(end)*regLen;
  positionMid=mid*regLen;

  strncpy(t1,source+positionIni,itemLen);
  t1[itemLen]=0;

  strncpy(t2,source+positionEnd,itemLen);
  t2[itemLen]=0;

  strncpy(t3,source+positionMid,itemLen);
  t3[itemLen]=0;


  if (strcmp(t1,item)==0)
  {
    return positionIni;
  }
  if (strcmp(t2,item)==0)
  {
     return positionEnd;
  }
  if (strcmp(t3,item)==0)
  {
    return positionMid;
  }
  if ((positionIni==0) && (positionMid==0) && (positionEnd==0))
    return -1;
  if ((positionIni==positionMid) && (positionEnd-regLen==positionIni))
    return -1;
  if (strcmp(t3,item)>0)
    return (findReg(source,item,start,mid,regLen,itemLen));
  else
    if (strcmp(t3,item)<0)
      return (findReg(source,item,mid,end,regLen,itemLen));
  return -1;
}
