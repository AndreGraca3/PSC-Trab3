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
#include "free_all_structs.h"

void free_all_structs(Products *productsStruct, Users *usersStruct, Carts *cartsStruct) {
	size_t i;
	for(i = 0; i < productsStruct->productsLen; ++i) {
		free((void *)productsStruct->products_array[i]->description);
		free((void *)productsStruct->products_array[i]->category);
		free(productsStruct->products_array[i]);
	}
	
	free(productsStruct->products_array);
	free(productsStruct);
	
	for(i = 0; i < usersStruct->usersLen; ++i) {
		free((void *)usersStruct->users_array[i]->name);
		free(usersStruct->users_array[i]);
	}
	
	free(usersStruct->users_array);
	free(usersStruct);
	
	for(i = 0; i < cartsStruct->cartsLen; ++i) {
		free(cartsStruct->carts_array[i]);
	}
	
	free(cartsStruct->carts_array);
	free(cartsStruct);
	
}
