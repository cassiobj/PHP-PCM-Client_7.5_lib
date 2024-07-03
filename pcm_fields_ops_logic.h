
#ifndef _PCM_FIELDS_OPS_LOGIC
#define _PCM_FIELDS_OPS_LOGIC


#include "pcm_structures.h"

#define PCM_SIZE_LINE_CUSTOM_FIELD  80

extern long  globalSizeCustomFields;
extern long globalSizeCustomOps;

extern field_spec *GLOBAL_CUSTOM_FIELDS;


extern char        *CUSTOM_FLD_OP_TABLE;

long pcm_get_file_size(FILE *file);
long  int pcm_array_find_custom_field( char *key, char **tabela,  field_spec **returnArray);
// long pcm_load_and_sort_field_spec(char **tabela, pcm_opcode **opcodeList, int *totField, int *totOpcode);
long pcm_load_and_sort_field_spec();
pcm_opcode *pcm_opcode_search(pcm_opcode *array, int size, const char *key) ;
int getFieldSpec(char *fieldName, field_spec **fields);



#endif