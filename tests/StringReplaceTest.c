#include "../StringReplace.h"
#include <string.h>
#include <assert.h>
#include <malloc.h>

int main() {
    char* result = replaceStr("replace each space with %20", " ", "%20");
    puts(result);
    assert(0 == strcmp(result, "replace%20each%20space%20with%20%20"));
    free(result);
    return 0;
}
