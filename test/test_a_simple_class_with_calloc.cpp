#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "Test1.h"

#include "smartall.h"

void Test1::do_something() {

	tmp = calloc(1, 10);

}

Test1::Test1() {

	tmp = NULL;

}


Test1::~Test1() {

	if (tmp)
	{
  	    free(tmp);
	}

}

int main() {

	Test1 *v1 = New(Test1);
	Test1 *v2 = New(Test1);

        v2->do_something();

	delete v1;
    delete v2;

#ifdef SMARTALLOC
	fprintf(stderr,
           "No orphaned buffer messages should follow this line.\n");
#endif
	sm_dump(1);

	return 0;


}
