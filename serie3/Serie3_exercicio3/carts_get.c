#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "http_get_json_data.h"
#include "carts_get.h"


/*typedef struct {
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
} Carts;*/

//const char *urlCarts = "https://dummyjson.com/carts";

Carts *carts_get() {
	const char *urlCarts = "https://dummyjson.com/carts";
	json_error_t error;
	json_t *root = http_get_json_data(urlCarts);
	size_t index1;
	json_t *value1;
	
	if (!json_is_object(root)) {
		fprintf(stderr, "***error: on line %d: %s\n", error.line, error.text);
		//goto cleanup_1;
	}
	
	json_t *carts_obj = json_object_get(root, "carts");
	if ( !json_is_array(carts_obj)) {
		fprintf(stderr, "carts not found\n");
		goto cleanup_2;
	}
	
	size_t carts_array_size = json_array_size(carts_obj);
	
	Carts *carts;
	size_t sizeOfEmptyCart = sizeof (carts->carts_array[0]->products[0]) + sizeof (Cart);
	carts = malloc(carts_array_size * sizeOfEmptyCart + sizeof (size_t));
	
	carts->carts_array = malloc(sizeOfEmptyCart);
	carts->cartsLen = carts_array_size;
	
	json_array_foreach(carts_obj, index1, value1) {
		size_t index2;
		json_t *value2;
		
		json_t *user_id_obj = json_object_get(value1, "userId");
		if ( !json_is_integer(user_id_obj)) {
			fprintf(stderr, "userId not found\n");
			goto cleanup_2;
		}
		json_t *n_products_obj = json_object_get(value1, "totalProducts");
		if ( !json_is_integer(n_products_obj)) {
			fprintf(stderr, "totalProducts not found\n");
			goto cleanup_2;
		}
		json_t *products_obj = 0;
		products_obj = json_object_get(value1, "products");
		if ( !json_is_array(products_obj)) {
			fprintf(stderr, "products not found\n");
			goto cleanup_2;
		}
		
		size_t totalProducts = json_integer_value(n_products_obj);
		
		Cart *cart;
		size_t sizeOfCartProduct = totalProducts * sizeof cart->products[index1] + sizeof (Cart);
		cart = malloc(sizeOfCartProduct);
		
		cart->user_id = json_integer_value(user_id_obj);
		cart->n_products = totalProducts;
		
		json_array_foreach(products_obj, index2, value2) {
			json_t *id_obj = json_object_get(value2, "id");
			if ( !json_is_integer(id_obj)) {
				fprintf(stderr, "id not found\n");
				goto cleanup_2;
			}
			json_t *quantity_obj = json_object_get(value2, "quantity");
			if ( !json_is_integer(quantity_obj)) {
				fprintf(stderr, "quantity not found\n");
				goto cleanup_2;
			}
			
			cart->products[index2].id = json_number_value(id_obj);
			cart->products[index2].quantity = json_integer_value(quantity_obj);
		}
		size_t reallocatedSizeOfCart = (index1 + 1) * (sizeOfCartProduct);
		carts = realloc(carts, reallocatedSizeOfCart + sizeof (size_t));
		carts->carts_array = realloc(carts->carts_array, reallocatedSizeOfCart);
		carts->carts_array[index1] = cart;
	}
	
cleanup_2:
	json_decref(root);
	
	return carts;
}
