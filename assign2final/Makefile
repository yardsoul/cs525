
test_assign2_1: test_assign2_1.o storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o
	gcc -o testOutput test_assign2_1.o storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o

test_assign2_1.o: test_assign2_1.c test_helper.h dberror.h storage_mgr.h buffer_mgr.h buffer_mgr_stat.h
	gcc -c test_assign2_1.c

storage_mgr.o: storage_mgr.c storage_mgr.h
	gcc -c storage_mgr.c

dberror.o: dberror.c dberror.h
	gcc -c dberror.c

buffer_mgr.o: buffer_mgr.c buffer_mgr.h
	gcc -c buffer_mgr.c

buffer_mgr_stat.o: buffer_mgr_stat.c buffer_mgr_stat.h
	gcc -c buffer_mgr_stat.c

clean: 
	rm testOutput *.o *.bin