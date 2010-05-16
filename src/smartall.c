/*

			 S M A R T A L L O C
			Smart Memory Allocator

	Evolved   over	 several  years,  starting  with  the  initial
	SMARTALLOC code for AutoSketch in 1986, guided	by  the  Blind
	Watchbreaker,  John  Walker.  Isolated in this general-purpose
	form in  September  of	1989.	Updated  with  be  more  POSIX
	compliant  and	to  include Web-friendly HTML documentation in
	October  of  1998  by  the  same  culprit.    For   additional
	information and the current version visit the Web page:

		  http://www.fourmilab.ch/smartall/

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <pthread.h>

/*LINTLIBRARY*/

#ifdef SMARTALLOC

#define P(x) pthread_mutex_lock(&x)
#define V(x) pthread_mutex_unlock(&x)

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum {False = 0, True = 1} Boolean;

#define EOS      '\0'              /* End of string sentinel */
#define sm_min(a, b) ((a) < (b) ? (a) : (b))

/*  Queue data structures  */

/*  General purpose queue  */

struct queue {
	struct queue *qnext,	   /* Next item in queue */
		     *qprev;	   /* Previous item in queue */
};

/*  Memory allocation control structures and storage.  */

struct abufhead {
	struct queue abq;	   /* Links on allocated queue */
	unsigned ablen; 	   /* Buffer length in bytes */
	const char *abfname;		   /* File name pointer */
	unsigned short ablineno;   /* Line number of allocation */ 
};

static struct queue abqueue = {    /* Allocated buffer queue */
	&abqueue, &abqueue
};

static bool bufimode = False;   /* Buffers not tracked when True */

/*  Queue functions  */

static void qinsert(struct queue *qhead, struct queue *object);
static struct queue *qremove(struct queue *qhead);
static struct queue *qdchain(struct queue *qitem);

/*  Forward references */
#ifdef __cplusplus
extern "C" void sm_free(void *fp);
extern "C" void *sm_malloc(const char *fname, int lineno, unsigned int nbytes);
extern "C" void sm_dump(bool bufdump);
#endif

/*  SMALLOC  --  Allocate buffer, enqueing on the orphaned buffer
		 tracking list.  */

static void *smalloc(const char *fname, int lineno, unsigned int nbytes)
{
	char *buf;

	/* Note:  Unix	MALLOC	actually  permits  a zero length to be
	   passed and allocates a valid block with  zero  user	bytes.
	   Such  a  block  can	later  be expanded with realloc().  We
           disallow this based on the belief that it's better to  make
	   a  special case and allocate one byte in the rare case this
	   is desired than to miss all the erroneous occurrences where
	   buffer length calculation code results in a zero.  */

	assert(nbytes > 0);

	nbytes += sizeof(struct abufhead) + 1;
	if ((buf = (char *) malloc(nbytes)) != NULL) {
	   P(mutex);
	   /* Enqueue buffer on allocated list */
	   qinsert(&abqueue, (struct queue *) buf);
	   ((struct abufhead *) buf)->ablen = nbytes;
	   ((struct abufhead *) buf)->abfname = bufimode ? NULL : fname;
	   ((struct abufhead *) buf)->ablineno = (unsigned short) lineno;
	   /* Emplace end-clobber detector at end of buffer */
	   buf[nbytes - 1] = (((long) buf) & 0xFF) ^ 0xC5;
	   buf += sizeof(struct abufhead);  /* Increment to user data start */
	   V(mutex);
	}
	return (void *) buf;
}

/*  SM_FREE  --  Update free pool availability.  FREE is never called
		 except  through  this interface or by actuallyfree().
		 free(x)  is  defined  to  generate  a	call  to  this
		 routine.  */

void sm_free(void *fp)
{
	char *cp = (char *) fp;
	struct queue *qp;

	assert(cp != NULL);	   /* Better not release a null buffer, guy! */

	cp -= sizeof(struct abufhead);
	qp = (struct queue *) cp;

	P(mutex);
	/* The following assertions will catch virtually every release
           of an address which isn't an allocated buffer. */

	if (qp->qnext->qprev != qp) {  /* Validate queue links */
		V(mutex);
		assert(0);
	}
    if (qp->qprev->qnext != qp) {
    	V(mutex);
    	assert(0);
    }

	/* The following assertion detects storing off the  end  of  the
	   allocated  space in the buffer by comparing the end of buffer
	   checksum with the address of the buffer.  */

	if (((unsigned char *) cp)[((struct abufhead *) cp)->ablen - 1] !=
	   ((((long) cp) & 0xFF) ^ 0xC5)) {
		V(mutex);
		assert(0);
	}

	qdchain(qp);
	V(mutex);

	/* Now we wipe the contents of	the  just-released  buffer  with
           "designer  garbage"  (Duff  Kurland's  phrase) of alternating
	   bits.  This is intended to ruin the day for any miscreant who
           attempts to access data through a pointer into storage that's
	   been previously released. */

	memset(cp, 0xAA, (int) ((struct abufhead *) cp)->ablen);

	free(cp);
}

/*  SM_MALLOC  --  Allocate buffer.  NULL is returned if no memory
		   was available.  */

void *sm_malloc(const char *fname, int lineno, unsigned int nbytes)
{
	void *buf;

	if ((buf = smalloc(fname, lineno, nbytes)) != NULL) {

	   /* To catch sloppy code that assumes  buffers  obtained  from
	      malloc()	are  zeroed,  we  preset  the buffer contents to
              "designer garbage" consisting of alternating bits.  */

	   memset(buf, 0x55, (int) nbytes);
	}
	return buf;
}

/*  SM_CALLOC  --  Allocate an array and clear it to zero.  */

void *sm_calloc(const char *fname, int lineno, unsigned int nelem, unsigned int elsize)
{
	void *buf;

	if ((buf = smalloc(fname, lineno, nelem * elsize)) != NULL) {
	   memset(buf, 0, (int) (nelem * elsize));
	}
	return buf;
}

/*  SM_REALLOC	--  Adjust the size of a  previously  allocated  buffer.
                    Note  that  the trick of "resurrecting" a previously
		    freed buffer with realloc() is NOT supported by this
		    function.	Further, because of the need to maintain
		    our control storage, SM_REALLOC must always allocate
		    a  new  block  and	copy  the data in the old block.
		    This may result in programs which make heavy use  of
		    realloc() running much slower than normally.  */

void *sm_realloc(const char *fname, int lineno, void *ptr, unsigned int size)
{
	unsigned osize;
	void *buf;

	assert(size > 0);

	/*  If	the  old  block  pointer  is  NULL, treat realloc() as a
	   malloc().  SVID is silent  on  this,  but  many  C  libraries
	   permit this.  */

	if (ptr == NULL)
	   return sm_malloc(fname, lineno, size);

	/* If the old and new sizes are the same, be a nice guy and just
	   return the buffer passed in.  */

	osize = ((struct abufhead *) (((char *) ptr) -
	    	    sizeof(struct abufhead)))->ablen -
		(sizeof(struct abufhead) + 1);
	if (size == osize) {
	   return ptr;
	}

	/* Sizes differ.  Allocate a new buffer of the	requested  size.
           If  we  can't  obtain  such a buffer, act as defined in SVID:
	   return NULL from  realloc()	and  leave  the  buffer  in  PTR
	   intact.  */

	if ((buf = smalloc(fname, lineno, size)) != NULL) {
	   memcpy(buf, ptr, (int) sm_min(size, osize));
	   /* If the new buffer is larger than the old, fill the balance
              of it with "designer garbage". */
	   if (size > osize) {
	      memset(((char *) buf) + osize, 0x55, (int) (size - osize));
	   }

	   /* All done.  Free and dechain the original buffer. */

	   sm_free(ptr);
	}
	return buf;
}

/*  ACTUALLYMALLOC  --	Call the system malloc() function to obtain
			storage which will eventually be released
			by system or library routines not compiled
			using SMARTALLOC.  */

void *actuallymalloc(unsigned int size)
{
	return malloc(size);
}

/*  ACTUALLYCALLOC  --	Call the system calloc() function to obtain
			storage which will eventually be released
			by system or library routines not compiled
			using SMARTALLOC.  */

void *actuallycalloc(unsigned int nelem, unsigned int elsize)
{
	return calloc(nelem, elsize);
}

/*  ACTUALLYREALLOC  --  Call the system realloc() function to obtain
			 storage which will eventually be released
			 by system or library routines not compiled
			 using SMARTALLOC.  */

void *actuallyrealloc(void *ptr, unsigned int size)
{
	return realloc(ptr, size);
}

/*  ACTUALLYFREE  --  Interface to system free() function to release
		      buffers allocated by low-level routines. */

void actuallyfree(void *cp)
{
	free(cp);
}

/*  SM_DUMP  --  Print orphaned buffers (and dump them if BUFDUMP is
		 True). */

void sm_dump(bool bufdump)
{
	struct abufhead *ap;

	P(mutex);

	ap = (struct abufhead *) abqueue.qnext;

	while (ap != (struct abufhead *) &abqueue) {

	   if ((ap == NULL) ||
	       (ap->abq.qnext->qprev != (struct queue *) ap) || 
	       (ap->abq.qprev->qnext != (struct queue *) ap)) {
	      fprintf(stderr,
                 "\nOrphaned buffers exist.  Dump terminated following\n");
	      fprintf(stderr,
                 "  discovery of bad links in chain of orphaned buffers.\n");
	      fprintf(stderr,
                 "  Buffer address with bad links: %lx\n", (long) ap);
	      break;
	   }

	   if (ap->abfname != NULL) {
	      unsigned memsize = ap->ablen - (sizeof(struct abufhead) + 1);
	      char errmsg[512];

	      sprintf(errmsg,
                "Orphaned buffer:  %6u bytes allocated at line %d of %s\n",
		 memsize, ap->ablineno, ap->abfname
	      );
              fprintf(stderr, "%s", errmsg);
	      if (bufdump) {
		 unsigned llen = 0;
		 char *cp = ((char *) ap) + sizeof(struct abufhead);

		 errmsg[0] = EOS;
		 while (memsize) {
		    if (llen >= 16) {
                       strcat(errmsg, "\n");
		       llen = 0;
                       fprintf(stderr, "%s", errmsg);
		       errmsg[0] = EOS;
		    }
                    sprintf(errmsg + strlen(errmsg), " %02X",
		       (*cp++) & 0xFF);
		    llen++;
		    memsize--;
		 }
                 fprintf(stderr, "%s\n", errmsg);
	      }
	   }
	   ap = (struct abufhead *) ap->abq.qnext;
	}
	V(mutex);
}

/*  SM_STATIC  --  Orphaned buffer detection can be disabled  (for  such
		   items  as buffers allocated during initialisation) by
		   calling   sm_static(1).    Normal   orphaned   buffer
		   detection  can be re-enabled with sm_static(0).  Note
		   that all the other safeguards still apply to  buffers
		   allocated  when  sm_static(1)  mode is in effect.  */

void sm_static(int mode)
{
	bufimode = (bool) (mode != 0);
}

/*  Queue manipulation functions.  */


/*  QINSERT  --  Insert object at end of queue	*/

static void qinsert(struct queue *qhead, struct queue *object)
{
	assert(qhead->qprev->qnext == qhead);
	assert(qhead->qnext->qprev == qhead);

	object->qnext = qhead;
	object->qprev = qhead->qprev;
	qhead->qprev = object;
	object->qprev->qnext = object;
}


/*  QREMOVE  --  Remove object from queue.  Returns NULL if queue empty  */

static struct queue *qremove(struct queue *qhead)
{
	struct queue *object;

	assert(qhead->qprev->qnext == qhead);
	assert(qhead->qnext->qprev == qhead);

	if ((object = qhead->qnext) == qhead)
	   return NULL;
	qhead->qnext = object->qnext;
	object->qnext->qprev = qhead;
	return object;
}


/*  QDCHAIN  --  Dequeue an item from the middle of a queue.  Passed
		 the queue item, returns the (now dechained) queue item. */

static struct queue *qdchain(struct queue *qitem)
{
	assert(qitem->qprev->qnext == qitem);
	assert(qitem->qnext->qprev == qitem);

	return qremove(qitem->qprev);
}

#endif
