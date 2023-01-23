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

typedef struct {
	int id;
	float price;
	const char *description;
	const char *category;
} Product;

typedef struct {
	int id;
	const char *name;
} User;

typedef struct {
	int user_id;
	size_t n_products;
	struct {
		int id;
		size_t quantity;
	} products[];
} Cart;

typedef struct {
	Product **products_array; //equivalente a *products_ptr[]
	size_t productsLen;
} Products;

typedef struct {
	User **users_array;
	size_t usersLen;
} Users;

typedef struct {
	Cart **carts_array;
	size_t cartsLen;
} Carts;

const char *urlProducts = "https://dummyjson.com/products";
const char *urlUsers = "https://dummyjson.com/users";
const char *urlCarts = "https://dummyjson.com/carts";

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

Products* products_get() {
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

Users *users_get() {
	json_error_t error;
	json_t *root = http_get_json_data(urlUsers);
	size_t index;
	json_t *value;
	
	if (!json_is_object(root)) {
		fprintf(stderr, "***error: on line %d: %s\n", error.line, error.text);
		//goto cleanup_1;
	}
	
	json_t *users_obj = json_object_get(root, "users");
	if ( !json_is_array(users_obj)) {
		fprintf(stderr, "users not found\n");
		goto cleanup_2;
	}
	
	size_t users_array_size = json_array_size(users_obj);
	
	Users *users = malloc(users_array_size * sizeof (User) + sizeof (size_t));
	
	users->users_array = malloc(users_array_size * sizeof (User));
	users->usersLen = users_array_size;

	json_array_foreach(users_obj, index, value) {
		json_t *id_obj = json_object_get(value, "id");
		if ( !json_is_integer(id_obj)) {
			fprintf(stderr, "id not found\n");
			goto cleanup_2;
		}
		json_t *name_obj = json_object_get(value, "username");
		if ( !json_is_string(name_obj)) {
			fprintf(stderr, "username not found\n");
			goto cleanup_2;
		}
		
		User *user = malloc(sizeof (User));
		
		user->id = json_integer_value(id_obj);
		user->name = strdup(json_string_value(name_obj));
		
		users->users_array[index] = user;
	}
	
cleanup_2:
	json_decref(root);
	
	return users;
	
}


Carts *carts_get() {
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


int main(int argc, char *argv[]) {	
	FILE *productsFile;
	FILE *usersFile;
	FILE *cartsFile;
	
	size_t i = 0;
	
	Products *all_products = products_get();
	Users *all_users = users_get();
	Carts *all_carts = carts_get();
	
	productsFile = fopen("Products.csv", "w");
	
	fprintf(productsFile, "id, price, description, category\n");
	
	for(i = 0; i < all_products->productsLen; ++i) {
		fprintf(productsFile, "%d, %.3f, %s, %s\n", all_products->products_array[i]->id, all_products->products_array[i]->price, all_products->products_array[i]->description, all_products->products_array[i]->category); 
	}
	
	if(ferror(productsFile)) {
		printf("Error writing to file");
	}
	
	fclose(productsFile);
	
	usersFile = fopen("Users.csv", "w");
	
	fprintf(usersFile, "id, name\n");
	
	for(i = 0; i < all_users->usersLen; ++i) {
		fprintf(usersFile, "%d, %s\n", all_users->users_array[i]->id, all_users->users_array[i]->name); 
	}
	
	if(ferror(usersFile)) {
		printf("Error writing to file");
	}
	
	fclose(usersFile);
	
	cartsFile = fopen("Carts.csv", "w");
	
	fprintf(cartsFile, "userId, totalProducts, id, quantity\n");
	
	for(i = 0; i < all_carts->cartsLen; ++i) {
		for(size_t j = 0; j < all_carts->carts_array[i]->n_products; ++j) {
			fprintf(cartsFile, "%d, %ld, %d, %ld\n", all_carts->carts_array[i]->user_id, all_carts->carts_array[i]->n_products, all_carts->carts_array[i]->products[j].id, all_carts->carts_array[i]->products[j].quantity);
		}
	}
	
	if(ferror(cartsFile)) {
		printf("Error writing to file");
	}
	
	fclose(cartsFile);
	
	free_all_structs(all_products, all_users, all_carts);
	
}

