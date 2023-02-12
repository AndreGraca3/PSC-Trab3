#ifndef GET_CARTS_H
#define GET_CARTS_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "http_get_json_data.h"

typedef struct {
    int user_id;
    size_t n_products;
    struct {
        int id;
        size_t quantity;
    } products[];
} Cart;

typedef struct {
    Cart **carts_array;
    size_t cartsLen;
} Carts;

Carts *carts_get();

#endif
