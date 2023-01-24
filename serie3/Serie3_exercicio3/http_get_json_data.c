#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "http_get_json_data.h"

/*struct writeStruct {
	char *memory;
	size_t size;
};*/

static size_t my_write(char *ptr, size_t size, size_t nmemb, void *fd) {
	size_t realSize = size * nmemb;
	struct writeStruct *mem = (struct writeStruct *)fd;
	
	char *newMem = realloc(mem->memory, mem->size + realSize + 1);
	
	if(!newMem) {
		printf("not enough memory\n");
		return 0;
	}
	mem->memory = newMem;
	memcpy(&(mem->memory[mem->size]), ptr, realSize);
	mem->size += realSize;
	mem->memory[mem->size] = 0;
	
	return realSize;
}

json_t *http_get_json_data(const char *url) {
	json_t *root;
	char *data;
	struct writeStruct writtenMemory;
	
	writtenMemory.memory = malloc(1);
	writtenMemory.size = 0;
	
	curl_global_init(CURL_GLOBAL_DEFAULT);

	CURL *curl = curl_easy_init();
	if (curl != NULL) {
		curl_easy_setopt(curl, CURLOPT_URL, url);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &writtenMemory);

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		CURLcode res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
		if (CURLE_OK != res) {
			fprintf(stderr, "curl told us %d\n", res);
		}
	}
	curl_global_cleanup();
	
	data = writtenMemory.memory;
	
	json_error_t error;
	root = json_loads(data, 0, &error);
	
	free(writtenMemory.memory);
	
	return root;
}
