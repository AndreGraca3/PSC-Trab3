#ifndef HTTP_GET_JSON_DATA_H
#define HTTP_GET_JSON_DATA_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

struct writeStruct {
	char *memory;
	size_t size;
};

//static size_t my_write(char *ptr, size_t size, size_t nmemb, void *fd);
json_t *http_get_json_data(const char *url);

#endif
