/*

        Definitions for the smart memory allocator

*/

#ifndef SMARTALL_H
#define SMARTALL_H

#ifdef __cplusplus
    extern "C" {
#endif


#ifdef SMARTALLOC
#undef  SMARTALLOC
#define SMARTALLOC SMARTALLOC

extern void *sm_malloc(const char *fname, int lineno, unsigned nbytes),
       *sm_calloc(const char *fname, int lineno, unsigned nelem, unsigned elsize),
       *sm_realloc(const char *fname, int lineno, void *ptr, unsigned size),
       *actuallymalloc(unsigned size),
       *actuallycalloc(unsigned nelem, unsigned elsize),
       *actuallyrealloc(void *ptr, unsigned size);

extern 	void sm_free(void *fp),
        actuallyfree(void *cp),
        sm_dump(bool bufdump),
        sm_static(int mode);

  
/* Redefine standard memory allocator calls to use our routines
   instead. */

#define free           sm_free
#define cfree          sm_free
#define malloc(x)      sm_malloc(__FILE__, __LINE__, (x))
#define calloc(n,e)    sm_calloc(__FILE__, __LINE__, (n), (e))
#define realloc(p,x)   sm_realloc(__FILE__, __LINE__, (p), (x))

/* Redefine strdup */
#define strdup(str) strcpy((char *)malloc(strlen((str))+1),(str))

#else

/* If SMARTALLOC is disabled, define its special calls to default to
   the standard routines.  */

#define actuallyfree(x)      free(x)
#define actuallymalloc(x)    malloc(x)
#define actuallycalloc(x,y)  calloc(x,y)
#define actuallyrealloc(x,y) realloc(x,y)
#define sm_dump(x)
#define sm_static(x)
#endif


#ifdef __cplusplus
    }
#endif

#ifdef __cplusplus

#ifdef SMARTALLOC

#define New(type) new(__FILE__, __LINE__) type

class SMARTALLOC
{
public:

void *operator new(size_t s, const char *fname, int line)
{
  void *p = sm_malloc(fname, line, s > sizeof(int) ? (unsigned int)s : sizeof(int));
  return p;
}
void *operator new[](size_t s, const char *fname, int line)
{
   void *p = sm_malloc(fname, line, s > sizeof(int) ? (unsigned int)s : sizeof(int));
   return p;
}
void  operator delete(void *ptr)
{
   free(ptr);
}
void  operator delete[](void *ptr, size_t /*i*/)
{
   free(ptr);
}

void  operator delete(void *ptr, const char * /*fname*/, int /*line*/)
{
   free(ptr);
}
void  operator delete[](void *ptr, size_t /*i*/, const char * /*fname*/, int /*line*/)
{
   free(ptr);
}


private:
void *operator new(size_t s) throw() { (void)s; return 0; }
void *operator new[](size_t s) throw() { (void)s; return 0; }
};


#else

#define New(type) new type

class SMARTALLOC
{
   public:
      void *operator new(size_t s)
      {
          return malloc(s);
      }
      void *operator new[](size_t s)
      {
          return malloc(s);
      }
      void  operator delete(void *ptr)
      {
          free(ptr);
      }
      void  operator delete[](void *ptr, size_t i)
      {
          free(ptr);
      }
};
#endif

#endif

#endif
