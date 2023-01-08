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

const char *urlProducts = "https://dummyjson.com/products";
const char *urlUsers = "https://dummyjson.com/users";
const char *urlCarts = "https://dummyjson.com/carts";

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

Product *products_get() {
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
	
	Product *products = malloc(products_array_size * sizeof *products);

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
		
		products[index].description = (char *)malloc(sizeof(char *) + 1);
		products[index].category = (char *)malloc(sizeof(char *) + 1);
		
		products[index].id = json_integer_value(id_obj);
		products[index].price = json_number_value(price_obj);
		products[index].description = json_string_value(description_obj);
		products[index].category = json_string_value(category_obj);
	}
	
	//printf("\nid:%d, price:%f, description:%s, category:%s\n", products[0].id, products[0].price, products[0].description, products[0].category); 
	
	//Product *returnedProducts = products;
	
	//free(products);
	
	
	
	return products;
	
cleanup_2:
	json_decref(root);
	
	
}

User *users_get() {
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
	
	User *users = malloc(users_array_size * sizeof *users);

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
		
		users[index].name = (char *)malloc(sizeof(char *) + 1);
		
		users[index].id = json_integer_value(id_obj);
		users[index].name = json_string_value(name_obj);
	}
	
	//printf("\nid:%d, price:%f, description:%s, category:%s\n", products[0].id, products[0].price, products[0].description, products[0].category); 
	
	//free(products);
	
	return users;
	
cleanup_2:
	json_decref(root);
	
	
}

Cart *carts_get() {
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
	
	Cart *carts = malloc(carts_array_size * sizeof *carts);
	

	json_array_foreach(carts_obj, index1, value1) {
		int i = 0;
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
		json_t *products_obj = json_object_get(value1, "products");
		if ( !json_is_array(products_obj)) {
			fprintf(stderr, "products not found\n");
			goto cleanup_2;
		}
		
		carts[index1].user_id = json_integer_value(user_id_obj);
		carts[index1].n_products = json_integer_value(n_products_obj);
		size_t totalProducts = carts[index1].n_products;
		
		/*size_t realSize = size * nmemb;
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
		
		return realSize;*/
		//carts[index1].products = malloc(totalProducts * sizeof(carts[index1]->products));
		//newMem = cartscarts[index1].products
		//newMem = maloc
		
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
			
			carts[index1].products[index2].id = json_integer_value(id_obj);
			int id_Product = carts[index1].products[index2].id;
			carts[index1].products[index2].quantity = json_integer_value(quantity_obj);
			int quant = carts[index1].products[index2].quantity;
			printf("\n%d", i);
			++i;
			
		}
	}
	
	//printf("\nid:%d, price:%f, description:%s, category:%s\n", products[0].id, products[0].price, products[0].description, products[0].category); 
	
	//free(products);
	
	return carts;
	
cleanup_2:
	json_decref(root);
	
}


int main(int argc, char *argv[]) {
	
	Product *all_products = products_get();
	User *all_users = users_get();
	Cart *all_carts = carts_get();
	
	printf("\nid: %d, price: %f, description: %s, category: %s\n", all_products[10].id, all_products[10].price, all_products[10].description, all_products[10].category); 
	
	//printf("\nid: %d, name: %s\n", all_users[0].id, all_users[0].name); 

	printf("\nuserId: %d, totalProducts: %ld, id: %d, quantity: %ld\n", all_carts[2].user_id, all_carts[2].n_products, all_carts[2].products[2].id, all_carts[2].products[2].quantity); 
	
	//free(all_products); //Free only removes the first element of all_products struct, so the remaining ones still exist in memory
	//free(all_users);
	
	//free(all_carts);
	
}

