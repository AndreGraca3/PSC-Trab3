#ifndef FREE_ALL_STRUCTS_H
#define FREE_ALL_STRUCTS_Hs

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "http_get_json_data.h"
#include "products_get.h"
#include "users_get.h"
#include "carts_get.h"

void free_all_structs(Products *productsStruct, Users *usersStruct, Carts *cartsStruct);

#endif

