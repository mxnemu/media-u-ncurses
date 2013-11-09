#pragma once

//#define STR(v) ##v
#define DEFAULT_CREATE_DESTROY_H(prefix)		\
    struct prefix * prefix ## _create();		\
    void prefix ## _destroy(struct prefix * this);

#define DEFAULT_CREATE_DESTROY(prefix)			\
    struct prefix * prefix ## _create() {		\
	const int size = sizeof(struct prefix);		\
	struct prefix * newValue = malloc(size);	\
	prefix ## _init(newValue);			\
	return newValue;				\
    }							\
    void prefix ## _destroy(struct prefix * this) {	\
	prefix ## _destroyMembers(this);		\
	free(this);					\
    }								

//const float app_version = 1.0;
