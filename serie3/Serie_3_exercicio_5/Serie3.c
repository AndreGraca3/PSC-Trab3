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

typedef struct{
	const char *user_name;
	float total_price;
	size_t tempLen;
} temp;

void showCarts(Users *all_users, Products *all_products, Carts *all_carts){
	temp tempCarts[all_users->usersLen];
	size_t tempLen = 0;
	for(size_t i = 0; i < all_carts->cartsLen; ++i){
		for(size_t j = 0; j< all_users->usersLen; ++j){
			if(all_carts->carts_array[i]->user_id == all_users->users_array[j]->id){
				//procura do id 
				tempCarts[tempLen].user_name = all_users->users_array[j]->name;
				tempLen += 1;
				tempCarts[tempLen - 1].total_price = 0.0;
				for(size_t k = 0; k < all_carts->carts_array[i]->n_products;++k){
					for(size_t l = 0; l < all_products->productsLen; l++){
						if(all_carts->carts_array[i]->products[k].id == all_products->products_array[l]->id){
							int N = all_carts->carts_array[i]->products[k].quantity;
							float price = all_products->products_array[l]->price;
							tempCarts[tempLen - 1].total_price += N * price;
						}
					}
				}	
			}
		}
	}
	
	for (size_t i = 0; i < tempLen; ++i)
		for (size_t j = 0; j < tempLen - i - 1; ++j)
			if ((tempCarts[j].total_price - tempCarts[j+1].total_price) > 0){
				swap(&tempCarts[j], &tempCarts[j + 1]);
				float temporario = tempCarts[j].total_price;
				tempCarts[j].total_price = tempCarts[j + 1].total_price;
				tempCarts[j + 1].total_price = temporario;
			}
				
	printf("Nome do usuário             Preço total\n");
	for(int i = 0; i < tempLen ; i++){
		printf("%s                   ",tempCarts[i].user_name);
		printf("%f\n",tempCarts[i].total_price);
	}
	return;
}


void showUsers(Users *all_users, Products *unused2, Carts *unused3){
	
	for (size_t i = 0; i < all_users->usersLen; ++i)
		for (size_t j = 0; j < all_users->usersLen - i - 1; ++j)
			if (strcmp(all_users->users_array[j]->name,all_users->users_array[j + 1]->name) > 0)
				swap(&all_users[j], &all_users[j + 1]);
	
	printf("id   name      id   name      id     name\n");
	for(int i = 0; i < all_users->usersLen;){ 
		printf("%d   %s      ",all_users->users_array[i]->id,all_users->users_array[i]->name);
		printf("%d   %s      ",all_users->users_array[i + 1]->id,all_users->users_array[i + 1]->name);
		printf("%d   %s\n",all_users->users_array[i + 2]->id,all_users->users_array[i + 2]->name);
		i += 3;
	}
	return;
}

void clearConsole(){
	for (int y = 0; y < 25; y++) //console is 80 columns and 25 lines
		printf("\n");
}

void swap(void **a, void **b) { // função auxiliar da função acima.
	void *t = *a;
	*a = *b;
	*b = t;
}

/*int main() {	
	FILE *productsFile;
	FILE *usersFile;
	FILE *cartsFile;
	
	size_t i = 0;
	
	Products *all_products = products_get();
	Users *all_users = users_get();
	Carts *all_carts = carts_get();
	
	char first_timeU = 0;
	char first_timeC = 0;
	char option;
	clearConsole();
	printf("introduce u for an alphabetic ordered list of users\n");
	printf("introduce c 'name'for carts lists\n");
	printf("introduce s for exit the application\n");
	
	while(scanf(" %c", &option) != -1){
		clearConsole();
		if(option == 'u'){
			first_timeU = 1;
			printf("You selected option u.\n");
			showUsers(all_users);
		}
		if(option == 'c'){
			first_timeC = 1;
			printf("You selected option c.\n");
			showCarts(all_users,all_products,all_carts);
		}
		if(option == 's') break;
	}
	*/
	
	/*
	 * Test program
	 */
	/* 
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
	*/
	
	//free_all_structs(all_products, all_users, all_carts);
	
//}


/*
 * Commands
 */

typedef struct command {
	void (*f) (Users *, Products *, Carts *);
	char c;
	struct command *next;
} Command;

static Command *commands = NULL;

void command_insert(char c, void (*f)(Users *, Products *, Carts *)) {
	Command *new_command = malloc(sizeof (Command));
	new_command->c = c;
	new_command->f = f;
	new_command->next = commands;
	commands = new_command;
}

void command_execute(char c, Users *usersParam, Products *productsParam, Carts *cartsParam) { // não é necessário o parâmetro param
	for (Command *p = commands; p != NULL; p = p->next)
		if (p->c == c) {
			p->f(usersParam, productsParam, cartsParam);
			return;
		}
}

/*void command_list(Users *unused, Products *unused, Carts *unused) {
	for (Command *p = commands; p != NULL; p = p->next)
		printf("%c%s\n", p->c, p->desc);
}*/

#include <dlfcn.h>

static void *handle;

static void commandNew(Users *unused1, Products *unused2, Carts *unused3) {
	handle = dlopen("libcontainer.so", RTLD_LAZY);
	if (handle == NULL) {
		fprintf(stderr, "%s\n", dlerror());
		return;
	}
	void (*f)(Users *, Products *, Carts *) = dlsym(handle, "command_function");
	if (f == NULL) {
		fprintf(stderr, "%s\n", dlerror());
		return;
	}
	char *c = dlsym(handle, "command_letter");
	if (c == NULL) {
		fprintf(stderr, "%s\n", dlerror());
		return;
	}
	command_insert(*c, f);
}

void leaveProgram(Users *users, Products *products, Carts *carts) {
	free_all_structs(products, users, carts);
	void *next;
	for (Command *p = commands; p != NULL; p = next) {
		next = p->next;
		//free(p->desc);
		free(p);
	}
	
	if (handle != NULL)
		dlclose(handle);
		
	exit(0);
}

int main() {
	Products *all_products = products_get();
	Users *all_users = users_get();
	Carts *all_carts = carts_get();
	printf("introduce u for an alphabetic ordered list of users\n");
	printf("introduce c 'name'for carts lists\n");
	printf("introduce s for exit the application\n");
	printf("introduce l for new command incorporation\n");
	char line[100];
	command_insert('s', leaveProgram);
	//command_insert('l', commandNew);
	command_insert('u', showUsers);
	command_insert('c', showCarts);

	while (1) {
		putchar('>');
		fgets(line, sizeof line, stdin);
		char *command = strtok(line, " \n");
		char *name = strtok(NULL, " \n");
		if (command != NULL)
			command_execute(*command, all_users, all_products, all_carts);
	}
}


