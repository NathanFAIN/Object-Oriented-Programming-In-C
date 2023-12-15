#include "my_obj.h"

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


char *string_c_str(string_t *string_obj) {
  return string_obj->__string;
}

void string_set(string_t *string_obj, char *str) {
  if (string_obj->__string) {
    free(string_obj->__string);
  }
  string_obj->__size = 0;
  while (str[string_obj->__size]) {
    string_obj->__size++;
  }
  string_obj->__max_size = string_obj->__size;
  string_obj->__string = malloc(sizeof(char) * (string_obj->__size + 1));
  for (size_t index = 0; index != string_obj->__size; index++) {
    string_obj->__string[index] = str[index];
  }
  string_obj->__string[string_obj->__size] = '\0';
}

size_t string_size(string_t *string_obj) {
  return string_obj->__size;
}

size_t string_max_size(string_t *string_obj) {
  return string_obj->__max_size;
}

string_t *string_copy(string_t *string_obj) {
  string_t *new_string_obj = create_string_t_obj();
  if (string_obj->__string) {
    new_string_obj->set(string_obj->c_str());
  }
  return new_string_obj;
}

bool string_empty(string_t *string_obj) {
  return string_obj->__string == NULL || string_obj->__string[0] == '\0';
}

void delete_string_obj(string_t *string_obj) {
  free(string_obj->c_str);
  free(string_obj->set);
  free(string_obj->copy);
  free(string_obj->size);
  free(string_obj->__string);
  free(string_obj->empty);
  free(string_obj);
}

string_t *create_string_t_obj(void) {
  string_t *string_obj = malloc(sizeof(string_t));

  char *string_obj_c_str__(void) {
    INIT_FUNC_OBJ_REF(string_obj, string_c_str);
  }
  CPY_NON_STATIC_FUNC_TO_OBJ(string_obj->c_str, string_obj_c_str__);

  void string_obj_set__(char *str) {
    INIT_FUNC_OBJ_REF(string_obj, string_set, str);
  }
  CPY_NON_STATIC_FUNC_TO_OBJ(string_obj->set, string_obj_set__);

  string_t *string_obj_copy__(void) {
    INIT_FUNC_OBJ_REF(string_obj, string_copy);
  }
  CPY_NON_STATIC_FUNC_TO_OBJ(string_obj->copy, string_obj_copy__);

  size_t string_obj_size__(void) {
    INIT_FUNC_OBJ_REF(string_obj, string_size);
  }
  CPY_NON_STATIC_FUNC_TO_OBJ(string_obj->size, string_obj_size__);

  size_t string_obj_empty__(void) {
  INIT_FUNC_OBJ_REF(string_obj, string_empty);
  }
  CPY_NON_STATIC_FUNC_TO_OBJ(string_obj->empty, string_obj_empty__);

  size_t string_obj_max_size__(void) {
    INIT_FUNC_OBJ_REF(string_obj, string_max_size);
  }
  CPY_NON_STATIC_FUNC_TO_OBJ(string_obj->max_size, string_obj_max_size__);

  string_obj->delete = delete_string_obj;
  string_obj->__string = NULL;
  string_obj->__size = 0;
  string_obj->__max_size = 0;
  string_obj->length = string_obj->size;

  return string_obj;
}