#include "my_obj.h"

void my_obj_func(my_obj_t *my_obj)
{
    if (my_obj != NULL) {
        printf("%i\n", my_obj->n);
    }
}