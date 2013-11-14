#include "EpisodeListToJson.h"
#include "EpisodeList.h"
#include <jansson.h>

char* EpisodeList_toJsonString(List* epPaths) {
    json_t* obj = json_object();
    json_t* array = json_array();
    for (ListNode* it = epPaths->first; it; ++it) {
	json_t* str = json_string(it->data);
	json_array_append(array, str);
    }
    json_object_set(obj, "episodes", array);
    return json_dumps(obj, JSON_COMPACT);
}
