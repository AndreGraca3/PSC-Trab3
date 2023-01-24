#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "http_get_json_data.h"
#include "users_get.h"

/*typedef struct {
	int id;
	const char *name;
} User;

typedef struct {
	User **users_array;
	size_t usersLen;
} Users;*/

//const char *urlUsers = "https://dummyjson.com/users";

Users *users_get() {
	const char *urlUsers = "https://dummyjson.com/users";
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
