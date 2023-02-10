#include <stdlib.h>
#include "products_get.h"
#include "users_get.h"
#include "carts_get.h"

extern Products *all_products;
extern Users *all_users;
extern Carts *all_carts;

void command_function(Users *users, Products *products, Carts *carts); // função por implementar

char command_letter = 'k';


