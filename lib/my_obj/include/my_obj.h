#ifndef MY_OBJ_H_
#define MY_OBJ_H_

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

typedef struct string_s string_t;

struct string_s
{
    char      *(*c_str)(void);
    void      (*set)(char *);
    string_t  *(*copy)(void);
    size_t    (*size)(void);
    size_t    (*length)(void);
    bool      (*empty)(void);
    size_t    (*max_size)(void);
    int       (*compare)(string_t *);
    int       (*find)(string_t *);
    int       (*rfind)(string_t *);
    string_t  *(*substr)(size_t,size_t);
    void      (*append)(string_t *);
    void      (*replace)(string_t *);
    void      (*insert)(size_t, string_t *);
    void      (*clear)(void);
    void      (*resize)(size_t);
    void      (*pop_back)();
    void      (*erase)(size_t,size_t);
    char      (*at)(size_t);
    char      (*back)(void);
    char      (*front)(void);
    void      (*delete)(string_t *);
    char      *__string;
    size_t    __size;
    size_t    __max_size;
};

void *cpy_func_size(void *fptr, size_t size);
void *cpy_func(void *fptr);

char *string_c_str(string_t *string_obj);
void string_set(string_t *string_obj, char *str);
void delete_string_obj(string_t *string_obj);
string_t *create_string_t_obj(void);

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

#define CREATE(obj_type) create_##obj_type##_obj()

#define DELETE(obj) obj->delete(obj)

#endif