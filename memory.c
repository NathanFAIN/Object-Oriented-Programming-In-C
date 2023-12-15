#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *cpy_func_size(void *fptr, size_t size) {
  const size_t pagesize = sysconf(_SC_PAGESIZE);
  void *func_page = (void *) (((uintptr_t)fptr) & ~(pagesize - 1));
  void *new_fptr = malloc(size);

  if (new_fptr == NULL) {
    return (NULL);
  }
  if (mprotect(func_page, pagesize, PROT_READ|PROT_WRITE|PROT_EXEC)) {
    free(new_fptr);
    return (NULL);
  }
  memcpy(new_fptr, fptr, size);
  func_page = (void *) (((uintptr_t)new_fptr) & ~(pagesize - 1));
  if (mprotect(func_page, pagesize, PROT_READ|PROT_WRITE|PROT_EXEC)) {
    free(new_fptr);
    return (NULL);
  }
  return new_fptr;
}

void *cpy_func(void *fptr) {
  return cpy_func_size(fptr, sysconf(_SC_PAGESIZE));
}

#define REGISTER_STATIC_FUNCTION_SIZE(func)     __typeof__(func) func __attribute__((noinline, section(#func "_size"))); \
                                                extern unsigned char __start_##func##_size[]; \
                                                extern unsigned char __stop_##func##_size[];
#define REGISTER_NON_STATIC_FUNCTION_SIZE(func) void __stop_##func##_size(void) {}
#define GET_STATIC_FUNCTION_SIZE(func)          (size_t)(__stop_##func##_size - __start_##func##_size)
#define GET_NON_STATIC_FUNCTION_SIZE(func)      (size_t)((void *)&__stop_##func##_size - (void *)&func)

#define INIT_FUNC_REF(func)      {void (*func_ref_init__)(void) = (void (*)(void))func; func_ref_init__();}

#define INIT_FUNC_GET_OBJ_ARG_REF(func, ...) func(__VA_ARGS__)

#define INIT_FUNC_OBJ_REF(obj, func, ...) _Pragma("GCC diagnostic push"); \
                                          _Pragma("GCC diagnostic ignored \"-Wreturn-type\""); \
                                          static __typeof__(obj) obj_ref = NULL; \
                                          if (obj_ref == NULL) {\
                                            obj_ref = obj; \
                                          } else { \
                                            return func(obj_ref, ##__VA_ARGS__); \
                                          } \


#define CREATE_METHOD(obj, method, method_ref, ...)  __typeof__(obj->method) __##method##_ref__(__VA_ARGS__) { \
                                                      static __typeof__(obj) obj_ref = NULL; \
                                                      if (obj_ref == NULL) {\
                                                        obj_ref = obj; \
                                                      } else { \
                                                        return method_ref(obj ...); \
                                                      } \
                                                    } \
                                                    REGISTER_NON_STATIC_FUNCTION_SIZE(__##method##_ref__); \
                                                    obj->method = cpy_func_size(&__##method##_ref__, GET_NON_STATIC_FUNCTION_SIZE(__##method##_ref__)); \
                                                    INIT_FUNC_REF(obj->method); \

#define CPY_NON_STATIC_FUNC_TO_OBJ(func_obj, func_ref)  _Pragma("GCC diagnostic pop"); \
                                                        REGISTER_NON_STATIC_FUNCTION_SIZE(func_ref); \
                                                        func_obj = cpy_func_size(&func_ref, GET_NON_STATIC_FUNCTION_SIZE(func_ref)); \
                                                        INIT_FUNC_REF(func_obj);\

#define TEST_ADD_METHOD(obj, method, func, type, ...)  type string_obj_set__(__VA_ARGS__) { \
                                                        INIT_FUNC_OBJ_REF(obj, func, string); \
                                                      } \
                                                      CPY_NON_STATIC_FUNC_TO_OBJ(obj->method, string_obj_set__); \



typedef struct string_s string_t;

struct string_s
{
    char      *(*c_str)(void);
    void      (*set)(char *);
    char      *string;
};


char *string_c_str(string_t *string_obj) {
  return string_obj->string;
}

void string_set(string_t *string_obj, char *str) {
  string_obj->string = str;
}

void destroy_string_obj(string_t *string_obj) {
  free(string_obj->c_str);
  free(string_obj->set);
  free(string_obj);
}

string_t *create_string_obj(void) {
  string_t *string_obj = malloc(sizeof(string_t));

  char *string_obj_c_str__(void) {
    INIT_FUNC_OBJ_REF(string_obj, string_c_str);
  }
  CPY_NON_STATIC_FUNC_TO_OBJ(string_obj->c_str, string_obj_c_str__);

  // void string_obj_set__(char *string) {
  //   INIT_FUNC_OBJ_REF(string_obj, string_set, string);
  // }
  // CPY_NON_STATIC_FUNC_TO_OBJ(string_obj->set, string_obj_set__);
  TEST_ADD_METHOD(string_obj, set, string_set, void, char *string)

  return string_obj;
}

void callee() {
  printf("test\n");
}

long long sub(int x, int y)
{
    return x - y;
}

#define CFI_DEF_CFA_OFFSET 16ull

void get_args (int arg1, ...)
{
    register int rdi __asm__ ("rdi"); // 1st arg
    register int rsi __asm__ ("rsi");
    register int rdx __asm__ ("rdx");
    register int rcx __asm__ ("rcx");
    register int r8  __asm__ ("r8" );
    register int r9  __asm__ ("r9" ); // 6th arg

    printf("%d %d %d %d %d %d\n", rdi, rsi, rdx, rcx, r8, r9);

    uint64_t frame_pointer = (uint64_t)__builtin_frame_address(0) + CFI_DEF_CFA_OFFSET;
    printf("%d\n", *((int*)frame_pointer)); // 1st stack argument
    frame_pointer += 8ull; // going to the next
    printf("%d\n", *((int*)frame_pointer)); // and so on ...
}

void get_args_bis (int arg1, ...)
{
    uint64_t frame_pointer = (uint64_t)__builtin_frame_address(0) + CFI_DEF_CFA_OFFSET;
    printf("%d\n", *((int*)frame_pointer)); // 1st stack argument
    frame_pointer += 8ull; // going to the next
    printf("%d\n", *((int*)frame_pointer)); // and so on ...
}

int power2( int num, int power )
{
  //  asm(
  //   "movl eax, num;"
  //   "movl ecx, power;"
  //  );
}

int main(void)
{
  string_t *string_obj = create_string_obj();
  
  string_obj->set("test 1");
  printf("string_obj: %s\n", string_obj->c_str());
  string_obj->set("test 2");
  printf("string_obj: %s\n", string_obj->c_str());

  destroy_string_obj(string_obj);

  asm("call %P0" : : "i"(callee));


//https://firexfly.com/clang-inline-assembly/
  int result = 0;

    asm("movl $200, %%edi;"
        "movl $500, %%esi;"
        "call sub;"
        "movl %%eax, %0;"
        : "=r"(result)
        :
        : "%edi", "%esi", "%eax"
    );

    printf("result = %d\n", result);

    // get_args_bis(666, 42, 64, 555, 1111, 8888, 7777, 4444);

  return 0;
}