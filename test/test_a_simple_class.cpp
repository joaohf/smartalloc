#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "smartall.h"

class Test1 : public SMARTALLOC
{
public:
	int a_test;

	void do_something();

};

int main() {

	Test1 *v1 = New(Test1);
	Test1 *v2 = New(Test1);

	delete v1;
        delete v2;

#ifdef SMARTALLOC
	fprintf(stderr,
           "No orphaned buffer messages should follow this line.\n");
#endif
	sm_dump(1);

	return 0;


}
