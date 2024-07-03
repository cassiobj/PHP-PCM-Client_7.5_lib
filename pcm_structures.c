
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  PHP-PCM-Client-lib
  PHP Extension interact with Oracle BRM
  Copyright Cassio Jemael 2024
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

#include <stdio.h>
#include <stdbool.h>
#include "pcm.h"
#include "cm_fm.h"

#include "pinlog.h"
#include "pin_mta.h"

//#include "ac_errs.h"
//#include "ac_custom_flds.h"


#include <pthread.h>
#include "pcm_structures.h"

#include "php.h"
#include "ext/standard/info.h"
#include "php_pcm_client.h"
#include "pcm_client_arginfo.h"


/*typedef struct
{
	char field[64];
	char type[64];
	int index_array;
	int size;
	void *value;
	struct pcm_flist *sub_flist;
	struct pcm_flist *next_flist;
	int64 totalSizeKey;
} pcm_flist ;*/

PCM_ARRAY_CONN ARRAY_CONN[MAX_CONTEXT_CONN];


int addFlistElement(pcm_flist ***flist_in, char *field, char *type, int index_array, void *value)
{
	int i=0;
	pcm_flist **temp=NULL;
	int size_t = i+1; // tamamnho da estrutura

	size_t = i+1; //
	if (*flist_in == NULL && temp==NULL)
	{
		temp  = (pcm_flist **) malloc(size_t * sizeof(pcm_flist*));
	}
	else if (flist_in!=NULL)
	{
		temp  = (pcm_flist **) realloc(*flist_in, size_t * sizeof(pcm_flist*));
	}


}

int addFlistElement2(struct_flist ***flist_in, int64 line, char *key, char *action, int index, int field, int type, int size, void *value)
{
	int i=0;
	struct_flist **temp=NULL;
	int size_t=0;
	int size_k=0;

	size_t = i+1; // 0 day references today, then each element must point to a day
	if (*flist_in == NULL && temp==NULL)
	{
		temp  = (struct_flist **) malloc(size_t * sizeof(struct_flist*));
	}
	else if (flist_in!=NULL)
	{
		temp  = (struct_flist **) realloc(*flist_in, size_t * sizeof(struct_flist*));
	}

	for (i=0;i<=line; i++)
	{
		if ((line == i) && (temp)[i] == NULL) // alocanting memory for single element struct for a day
		{
			(temp)[i] = (struct_flist *) malloc( sizeof(struct_flist));
			memset((temp)[i], 0, sizeof(struct_flist));
			(temp)[i]->totalSizeKey=0;
		}

		if (line == i)
		{
			size_k = strlen(key)+1; // 1 references separator
			if ((temp)[i]->key == 0)
			{
				(temp)[i]->key = (char *) malloc (size_k * sizeof(char) + 1);
				(temp)[i]->key[0] = 0;
			}
			else
			{
				(temp)[i]->key = (char *) realloc ((temp)[i]->key, ((temp)[i]->totalSizeKey + size_k) * sizeof(char) + 1);
				(temp)[i]->key[(temp)[i]->totalSizeKey] = 0;
			}
			strcat((temp)[i]->key, key);
			strcat((temp)[i]->key, "|");
			(temp)[i]->totalSizeKey += size_k;
		}
	}




	*flist_in = temp;


	return 0;
}


int destroyFlistElement(struct_flist ***flist_in)
{
	int size_t = sizeof(struct_flist*);
	int64 i = 0;
	struct_flist **temp;

	for (i=0; (*flist_in)[i]!=NULL ; i++ )
	{
		if ((*flist_in)[i] != NULL)
		{
			if ((*flist_in)[i]->key != NULL)
				free((*flist_in)[i]->key);
		}
		i++;
	}


	free((*flist_in));


}

