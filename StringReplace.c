#include <string.h>
#include "List.h"

struct occurance {
    size_t prefixSize;
    char* prefix;
    char* position;
    char* followUp;
};

void freeThis_destroy(void* this) {
    free(this);
}

char* replaceStr(const char* data, const char* original, const char* replacement) {

    static const size_t occuranceSize = sizeof(struct occurance);    
    const size_t replacementSize = strlen(replacement);
    const size_t originalSize = strlen(original);
    
    List* occurances = List_create();

    size_t newSize = 0; 
    char* foundPt = NULL;
    char* pos = (char*)data;
    while (1) {
	foundPt = strstr(pos, original);

	if (foundPt) {
	    struct occurance* occ = malloc(occuranceSize);
	    occ->prefixSize = (size_t)foundPt - (size_t)pos;
	    occ->prefix = pos;
	    occ->position = foundPt;
	    occ->followUp = foundPt + originalSize;
	    List_pushBack(occurances, occ);
	    newSize += occ->prefixSize + replacementSize;
	    pos = foundPt + originalSize;
	} else {
	    break;
	}
    }

    if (!occurances->first) {
	List_destroyWithContent(occurances, freeThis);
	return NULL;
    }

    newSize += strlen(pos) +1;

    char* newString = malloc(newSize);
    char* insertPos = newString;
    for (ListNode* it = occurances->first; it; it = it->next) {
	struct occurance* occ = it->data;
	memcpy(insertPos, occ->prefix, occ->prefixSize);
	insertPos += occ->prefixSize;
	memcpy(insertPos, replacement, replacementSize);
	insertPos += replacementSize;
	if (!it->next) {
	    strcpy(insertPos, occ->followUp);
	}
    }
    
    List_destroyWithContent(occurances, freeThis);
    return newString;
}
