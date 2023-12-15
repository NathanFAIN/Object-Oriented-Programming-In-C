#include "my_obj.h"

int main(void)
{
    string_t *str = CREATE(string_t);
    str->set("test");
    printf("%s\n", str->c_str());
    printf("%li\n", str->size());
    printf("%li\n", str->length());
    printf("%i\n", str->empty());
    DELETE(str);
    return (0);
}