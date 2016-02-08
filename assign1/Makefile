
test_assign1_1: test_assign1_1.o storage_mgr.o dberror.o
	gcc -o testOutput test_assign1_1.o storage_mgr.o dberror.o 

test_assign1_1.o: test_assign1_1.c test_helper.h dberror.h storage_mgr.h
	gcc -c test_assign1_1.c

storage_mgr.o: storage_mgr.c storage_mgr.h
	gcc -c storage_mgr.c

dberror.o: dberror.c dberror.h
	gcc -c dberror.c

clean: 
	rm testOutput *.o *.bin
