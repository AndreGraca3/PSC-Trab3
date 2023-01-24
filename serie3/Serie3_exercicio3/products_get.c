#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "http_get_json_data.h"
#include "products_get.h"

/*typedef struct {
	int id;
	float price;
	const char *description;
	const char *category;
} Product;

typedef struct {
	Product **products_array; //equivalente a *products_ptr[]
	size_t productsLen;
} Products;*/

//const char *urlProducts = "https://dummyjson.com/products";

Products* products_get() {
	const char *urlProducts = "https://dummyjson.com/products";
	json_error_t error;
	json_t *root = http_get_json_data(urlProducts);
	size_t index;
	json_t *value;
	
	if (root == NULL) {
		fprintf(stderr, "***error: on line %d: %s\n", error.line, error.text);
		//goto cleanup_1;
	}
	
	json_t *products_obj = json_object_get(root, "products");
	if ( !json_is_array(products_obj)) {
		fprintf(stderr, "products not found\n");
		goto cleanup_2;
	}
	
	size_t products_array_size = json_array_size(products_obj);
	
	Products *products = malloc(products_array_size * sizeof (Product) + sizeof (size_t));
	products->products_array = malloc(products_array_size * sizeof (Product));
	products->productsLen = products_array_size;

	json_array_foreach(products_obj, index, value) {
		json_t *id_obj = json_object_get(value, "id");
		if ( !json_is_integer(id_obj)) {
			fprintf(stderr, "id not found\n");
			goto cleanup_2;
		}
		json_t *price_obj = json_object_get(value, "price");
		if ( !json_is_integer(price_obj)) {
			fprintf(stderr, "price not found\n");
			goto cleanup_2;
		}
		json_t *description_obj = json_object_get(value, "description");
		if ( !json_is_string(description_obj)) {
			fprintf(stderr, "description not found\n");
			goto cleanup_2;
		}
		json_t *category_obj = json_object_get(value, "category");
		if ( !json_is_string(category_obj)) {
			fprintf(stderr, "category not found\n");
			goto cleanup_2;
		}
		Product *product = malloc(sizeof(Product));
		
		product->id = json_integer_value(id_obj);
		product->price = json_number_value(price_obj);
		product->description = strdup(json_string_value(description_obj));
		product->category = strdup(json_string_value(category_obj));
		
		products->products_array[index] = product;
		
		
	}
	
cleanup_2:
	json_decref(root);
	
	return products;
	
}
