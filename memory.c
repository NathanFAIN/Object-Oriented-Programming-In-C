#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int foo() {
  return 42;
}

void *cpyFunc(void *fptr) {
  size_t pagesize = sysconf(_SC_PAGESIZE);
  void *func_page = (void *) (((uintptr_t)fptr) & ~(pagesize - 1));
  void *new_fptr = malloc(pagesize);

  if (mprotect(func_page, pagesize, PROT_READ|PROT_WRITE|PROT_EXEC)) {
    perror("mprotect");
  }
  memcpy(new_fptr, fptr, pagesize);
  func_page = (void *) (((uintptr_t)new_fptr) & ~(pagesize - 1));
  if (mprotect(func_page, pagesize, PROT_READ|PROT_WRITE|PROT_EXEC)) {
    perror("mprotect");
  }
  return new_fptr;
}

__attribute__((noinline, section("mysection1"))) void *createFunc(int n) {
  int fun() {
    return n;
  }
  return cpyFunc(fun);
}

int i = 0;

void test_func (void)
{
    i++;
    i++;
    i++;
    //__attribute__((noinline, section(__FUNCTION__)))
    //__attribute__((noinline, section("mysection2"))) 
}

extern test_func __attribute__((noinline, section("mysection2")));

int main (int argc, char *argv[])
{
  extern unsigned char __start_mysection1[];
  extern unsigned char __stop_mysection1[];

  printf ("Func len: %lu\n", __stop_mysection1 - __start_mysection1);

  extern unsigned char __start_mysection2[];
  extern unsigned char __stop_mysection2[];

  printf ("Func len: %lu\n", __stop_mysection2 - __start_mysection2);

  void *memfun = cpyFunc(&foo);
  int (*fptr)();
  fptr = memfun;
  printf("%d @ %p\n", (*fptr)(), (void *)fptr);
  memfun = createFunc(10);
  fptr = memfun;
  printf("%d @ %p\n", (*fptr)(), (void *)fptr);
  return 0;
}

/*
int function1(int x){ 
   return x-5;
}   

int function2(int x){ 
  //printf("hello world");
  int k=32;
  int l=40;
  return x+5+k+l;
}   


int main(){

    int tmp = 0;
    int function2(int x){ 
        //printf("hello world");
        int k=32;
        int l=40;
        return x+5+k+l;
    }  

  int diff = 4096;//((void *)&function2 - (void *)&tmp);
  printf("pagesize: %d, diff: %d\n",getpagesize(),diff);

  int (*fptr)(int);

  void *memfun = malloc(4096);

  if (mprotect(memfun, 4096, PROT_READ|PROT_EXEC|PROT_WRITE) == -1) {
      perror("mprotect");
  }   

  fptr = &function1;
  printf("native: %d\n",(*fptr)(6));
  fptr = memfun;
  memcpy(memfun, (const void*)&function1, diff);
  int n = (*fptr)(6);
  //printf("memory: %d\n", n);
  fptr = &function1;
  printf("native: %d\n",(*fptr)(6));

  free(memfun);
  return 0;
}
*/