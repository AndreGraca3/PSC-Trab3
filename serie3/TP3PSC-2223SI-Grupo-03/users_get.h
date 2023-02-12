#ifndef GET_USERS_H
#define GET_USERS_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "http_get_json_data.h"

typedef struct {
	int id;
	const char *name;
} User;

typedef struct {
	User **users_array;
	size_t usersLen;
} Users;

Users *users_get();

#endif
