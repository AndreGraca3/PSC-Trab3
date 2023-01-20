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
	Product **products_ptr; //equivalente a *products_ptr[]
	size_t productsLen;
} Products;

typedef struct {
	User **users_ptr;
	size_t usersLen;
} Users;

typedef struct {
	Cart **carts_ptr;
	size_t cartsLen;
} Carts;

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
	
	printf("\nOla1");
	
	Products *products_array = malloc(products_array_size * sizeof (Products)); //memoria desnecessaria
	//products_array->products_ptr[products_array_size]; //isto pode ser desnecessario
	//Product **ptr = products_array->products_ptr;
	products_array->productsLen = products_array_size;
	
	printf("\nOla2");
	printf("\narray size: %ld", products_array->productsLen);
	
	//Product *products = malloc(products_array_size * sizeof(Product));

	json_array_foreach(products_obj, index, value) {
		
		printf("\nOla3");
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
		printf("\nOla4");
		Product *product = malloc(sizeof(Product));
		printf("\n product: %p", product);
		printf("\n products_array: %p", products_array->products_ptr);
		//products_array->products_ptr[index] = product;
		//product = malloc(sizeof(Product));
		
		printf("\nAdeus");
		product->id = json_integer_value(id_obj);
		product->price = json_number_value(price_obj);
		product->description = strdup(json_string_value(description_obj));
		product->category = strdup(json_string_value(category_obj));
		printf("\nid: %d, price: %f, description: %s, category: %s\n", product->id, product->price, product->description, product->category);
		/*products_array->products_ptr[index]->id = json_integer_value(id_obj);
		products_array->products_ptr[index]->price = json_number_value(price_obj);
		products_array->products_ptr[index]->description = strdup(json_string_value(description_obj));
		products_array->products_ptr[index]->category = strdup(json_string_value(category_obj));*/
		
		//*(products_array->products_ptr + index) = &product;
		//ptr++;
		*(products_array->products_ptr + index) = product;
		printf("\nOla5");
		//printf("\nid: %d, price: %f, description: %s, category: %s\n", products_array->products_ptr[0]->id, products_array->products_ptr[0]->price, products_array->products_ptr[0]->description, products_array->products_ptr[0]->category);
		printf("\n");
		printf("\n");
		printf("\n");
		printf("\n");
		
	}
	
	//printf("\nid:%d, price:%f, description:%s, category:%s\n", products[0].id, products[0].price, products[0].description, products[0].category); 
	
	//Product *returnedProducts = products;
	
	//free(products);
	
	//free(root);
	
cleanup_2:
	json_decref(root);
	
	return products_array;
	
}

/*User *users_get() {
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
	
	User *users = malloc(users_array_size * sizeof (User));

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
		
		users[index].id = json_integer_value(id_obj);
		users[index].name = strdup(json_string_value(name_obj));
	}
	
	//printf("\nid:%d, price:%f, description:%s, category:%s\n", products[0].id, products[0].price, products[0].description, products[0].category); 
	
cleanup_2:
	json_decref(root);
	
	return users;
	
}*/


/*Cart *carts_get() {
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
	
	json_t *firstElementOfCart = json_array_get(carts_obj, 0L);
	
	json_t *total_products_obj = json_object_get(firstElementOfCart, "totalProducts");
	if ( !json_is_integer(total_products_obj)) {
		fprintf(stderr, "totalProducts not found\n");
		goto cleanup_2;
	}
	
	Cart *carts = malloc((json_integer_value(total_products_obj) * sizeof carts->products[0] + sizeof *carts) * carts_array_size);
	
	
	//Cart *cart_products;


	int idx = 0;
	json_array_foreach(carts_obj, index1, value1) {
		int idx = 0;
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
		
		//carts[index1].user_id = json_integer_value(user_id_obj);
		//carts[index1].n_products = json_integer_value(n_products_obj);
		//size_t totalProducts = carts[index1].n_products;
		
		//carts[index1].products = malloc(totalProducts * sizeof(carts[index1]->products));
		//newMem = cartscarts[index1].products
		//newMem = maloc
		
		//size_t totalProducts = json_integer_value(n_products_obj);
		
		//carts_products = malloc(sizeof *carts + (totalProducts * sizeof carts->products[0]));
		
		//int sizeOfACart = sizeof *carts + (totalProducts * sizeof carts->products[0]);
		
		//carts[index1].user_id = json_integer_value(user_id_obj);
		//carts[index1].n_products = json_integer_value(n_products_obj);
		//carts[index1].products = 
		
		printf("\n indx1: %ld", index1);
		printf("\n");
		
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
			
			carts[index1].user_id = json_integer_value(user_id_obj);
			carts[index1].n_products = json_integer_value(n_products_obj);
			S
			carts[index1].products[index2].id = json_number_value(id_obj);
			printf("\n user_id: %d, id: %d, index1: %ld, index2: %ld, idx: %d", carts[0].user_id, carts[0].products[index2].id, index1, index2, idx);
			carts[index1].products[index2].quantity = json_integer_value(quantity_obj);
			//++idx;
		}
		//printf("\nuserId: %d, totalProducts: %ld, id: %d, quantity: %ld\n", carts[0].user_id, carts[0].n_products, carts[0].products->id, carts[0].products->quantity);
		printf("\n");
		printf("\n indx1: %ld", index1);
		//++idx;
		int j;
	}
	
	//printf("\nuserId: %d, totalProducts: %ld, id: %d, quantity: %ld\n", carts[0].user_id, carts[0].n_products, carts[0].products->id, carts[0].products->quantity);
	
cleanup_2:
	json_decref(root);
	
	return carts;
	
}*/


int main(int argc, char *argv[]) {
	int result = -1;	
	size_t i = 0;
	//json_t *root;
	/*if (argc != 2) {
		fprintf(stderr, "usage: %s <url>\n", argv[0]);
		return 1;
	}*/
	
	//json_error_t error;
	//root = http_get_json_data(argv[1]);
	/*
	if (!json_is_object(root)) {
		fprintf(stderr, "***error: on line %d: %s\n", error.line, error.text);
		goto cleanup_1;
	}*/
	
	//Product *all_products = products_get();
	//User *all_users = users_get();
	//Cart *all_carts = carts_get();
	
	Products *all_products = products_get();
	
	//printf("\nid: %d, price: %f, description: %s, category: %s\n", all_products->products_ptr[10]->id, all_products->products_ptr[10]->price, all_products->products_ptr[10]->description, all_products->products_ptr[10]->category); 
	
	//printf("\nid: %d, name: %s\n", all_users[0].id, all_users[0].name); 

	//printf("\nuserId: %d, totalProducts: %ld, id: %d, quantity: %ld\n", all_carts[0].user_id, all_carts[0].n_products, all_carts[0].products[0].id, all_carts[0].products[0].quantity);
	
	
	int products_num_elem = 30;
	//int users_num_elem = 30;
	
	for(i = 0; i < products_num_elem; ++i) {
		free((void *)all_products->products_ptr[i]->description);
		free((void *)all_products->products_ptr[i]->category);
	}
	
	free(all_products);
	
	/*for(i = 0; i < users_num_elem; ++i) {
		free((void *)all_users[i].name);
	}*/
	
	//free(all_users);
	
	/*for(size_t i = 0; i < products_num_elem; ++i) {
		free((void *)all_products[i].description);
		free((void *)all_products[i].category);
	}*/
	
	//free(all_carts);
	
	/*json_t *title_obj = json_object_get(root, "title");
	if ( !json_is_string(title_obj)) {
		fprintf(stderr, "title not found\n");
		goto cleanup_2;
	}
	printf("%s\n", json_string_value(title_obj));*/
	
/*cleanup_2:
	json_decref(root);

cleanup_1:
	return result;
	*/
	//free(root);
}

