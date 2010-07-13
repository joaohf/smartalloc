#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "smartall.h"

int main() {

	char *s_test[100];
	int i;

	memset(s_test, 0, sizeof(s_test));

	for (i = 0; i < 100; i++)
	{
		s_test[i] = (char *) calloc(1, 10);;

		fprintf(stderr,
	           "Alloc %d position\n", i);

	}

	sm_dump(0);

	for (i = 0; i < 100; i++)
	{
		if(s_test[i])
		{
		  free(s_test[i]);
		  s_test[i] = NULL;
			fprintf(stderr,
		           "Dealloc %d position\n", i);

		}
	}


#ifdef SMARTALLOC
	fprintf(stderr,
           "No orphaned buffer messages should follow this line.\n");
#endif

	sm_dump(1);

	return 0;

}
