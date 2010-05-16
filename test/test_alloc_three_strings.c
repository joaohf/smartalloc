#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "smartall.h"

char* buf() {
	char * _buf = (char *) malloc(sizeof(char *) * 50);
	strcpy(_buf, "Test 1234");
	return _buf;
}

int main() {

	char *s[3];

	s[0] = buf();
	s[1] = buf();
	s[2] = buf();

	sm_dump(1);

#ifdef SMARTALLOC
	fprintf(stderr,
           "No orphaned buffer messages should follow this line.\n");
#endif

	free(s[0]);
	free(s[1]);
	free(s[2]);

	sm_dump(0);

	return 0;

}
