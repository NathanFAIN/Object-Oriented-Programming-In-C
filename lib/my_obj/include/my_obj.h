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


typedef struct my_obj_s my_obj_t;

void my_obj_func(my_obj_t *);

struct my_obj_s
{
    int     n;
};

#endif