#ifndef GET_PRODUCTS_H
#define GET_PRODUCTS_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "http_get_json_data.h"

typedef struct {
	int id;
	float price;
	const char *description;
	const char *category;
} Product;

typedef struct {
	Product **products_array; 
	size_t productsLen;
} Products;

Products* products_get();

#endif
