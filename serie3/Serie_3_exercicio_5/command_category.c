#include <stdlib.h>
#include "products_get.h"
#include "users_get.h"
#include "carts_get.h"

extern Products *all_products;
extern Users *all_users;
extern Carts *all_carts;

typedef struct{
	const char *category;
	int times;
} Category;

void command_function(Users *users, Products *products, Carts *carts){
	
	Category arr[products->productsLen];	
	arr[0].category = products->products_array[0]->category;
	arr[0].times = 1;
	size_t length = 1; // number of categories.
	
	for(int i = 1; i < products->productsLen; ++i){
		if(strcmp(arr[length - 1].category,products->products_array[i]->category) == 0 ){
			arr[length - 1].times += 1;
		}
		
		if(strcmp(arr[length - 1].category,products->products_array[i]->category) != 0){
			arr[length].category = products->products_array[i]->category;
			arr[length].times = 1;
			length++;
		}
	}	
		
	for (size_t i = 0; i < length; ++i)
		for (size_t j = 0; j < length - i - 1; ++j)
			if ((arr[j].times - arr[j+1].times) > 0){
				swap(&arr[j], &arr[j + 1]);
			}
		
	printf("Category          Ntimes\n");	
	for(int j = 0 ; j < length; ++j){
		printf("%s            ",arr[j].category);
		printf("%d\n",arr[j].times);
	}
	
	return;
};

char command_letter = 'k';


