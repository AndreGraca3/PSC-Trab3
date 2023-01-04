
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <curl/curl.h>
#include <jansson.h>
 
struct MemoryStruct {
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

const char *url1 = "https://dummyjson.com/products/1";
const char *url2 = "https://dummyjson.com/products";
const char *url3 = "https://dummyjson.com/users/1";


static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}
 
json_t * http_get_json_data(const char *url){
  CURL *curl_handle;
  CURLcode res;
  
  json_t *root;
 
  struct MemoryStruct chunk;
 
  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;    /* no data at this point */
 
  curl_global_init(CURL_GLOBAL_ALL);
 
  /* init the curl session */
  curl_handle = curl_easy_init();
 
  /* specify URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL,url);
 
  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
 
  /* some servers do not like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
 
  /* get it! */
  res = curl_easy_perform(curl_handle);
 
  /* check for errors */
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }
	else {
		/*
		* Now, our chunk.memory points to a memory block that is chunk.size
		* bytes big and contains the remote file.
		*
		* Do something nice with it!
		*/
    
		json_error_t error;
		root = json_loads(chunk.memory,JSON_DECODE_ANY, &error);
		if (!json_is_object(root)) {
			fprintf(stderr, "***error: on line %d: %s\n", error.line, error.text);
			return NULL;
		}

	}
	
	/* cleanup curl stuff */
	curl_easy_cleanup(curl_handle);
 
	free(chunk.memory);
 
	/* we are done with libcurl, so clean it up */
	curl_global_cleanup();
	
	return root;
}

Product *product_get(){
	json_t *root = http_get_json_data(url1);
	
	Product p;
		
	//if(root == NULL) return p; 
	
	//json_t *products_obj = json_array_get(root,"products"); // products
	//if(!json_is_object(products_obj)){
	//	fprintf(stderr, "products not found\n");
	//}
	//printf("products = %s\n",json_string_value(products_obj));
	
	
	json_t *id_obj = json_object_get(root, "id"); // id
	if ( !json_is_integer(id_obj)) {
		fprintf(stderr, "id not found\n");
	}
	p.id = json_integer_value(id_obj);
	printf("id = %d\n", p.id);
	
	json_t *price_obj = json_object_get(root, "price"); // price
	if (!json_is_integer(price_obj)) {
		fprintf(stderr, "price not found\n");
	}
	p.price = json_integer_value(price_obj);
	printf("price = %f\n", p.price);

	json_t *description_obj = json_object_get(root, "description"); //description
	if (!json_is_string(description_obj)) {
		fprintf(stderr, "description not found\n");
	}
	p.description = json_string_value(description_obj);
	printf("description = %s\n",p.description);

	json_t *category_obj = json_object_get(root, "category"); //category
	if (!json_is_string(category_obj)) {
		fprintf(stderr, "category not found\n");
	}
	p.category = json_string_value(category_obj);
	printf("category = %s\n",p.category);
	
	Product *ptr = (&p);
	
	return ptr;
}

User *user_get(){
	json_t *root = http_get_json_data(url3);

    User u;

    json_t *id_obj = json_object_get(root, "id"); // id
    if ( !json_is_integer(id_obj)) {
        fprintf(stderr, "id not found\n");
    }
    u.id = json_integer_value(id_obj);
    printf("id = %d\n", u.id);

    json_t *name_obj = json_object_get(root, "username"); // userName
    if( !json_is_string(name_obj)) {
        fprintf(stderr, "id not found\n");
    }
    u.name = json_string_value(name_obj);
    printf("name = %s\n", u.name);
    
	User *ptr = (&u);
	
	return ptr;
}

int main(){
	Product *p = product_get();
	User *u = user_get();
	return 0;
}
