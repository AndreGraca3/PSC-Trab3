serie3.exe: serie3
	./serie3
	
export: libcontainer.so
	export LD_LIBRARY_PATH=.

serie3: serie3.c libcontainer.so
	gcc serie3.c -o serie3 -L. -lcontainer -lcurl -ljansson

libcontainer.so: carts_get.o free_all_structs.o http_get_json_data.o products_get.o serie3.o users_get.o command_category.o
	gcc -shared -o libcontainer.so carts_get.o free_all_structs.o http_get_json_data.o products_get.o serie3.o users_get.o command_category.o

serie3.o: carts_get.c free_all_structs.c http_get_json_data.c products_get.c serie3.c users_get.c command_category.c
	gcc -c -fpic carts_get.c free_all_structs.c http_get_json_data.c products_get.c serie3.c users_get.c command_category.c
	
valgrind: serie3
	valgrind ./serie3
	

