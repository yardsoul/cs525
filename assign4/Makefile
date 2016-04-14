all: test_assign4_1 test_expr

test_assign4_1: test_assign4_1.o storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o expr.o record_mgr.o rm_serializer.o btree_mgr.o
	gcc -o testOutput test_assign4_1.o storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o expr.o record_mgr.o rm_serializer.o btree_mgr.o

test_assign4_1.o: test_assign4_1.c test_helper.h dberror.h storage_mgr.h buffer_mgr.h buffer_mgr_stat.h expr.h record_mgr.h btree_mgr.h
	gcc -c test_assign4_1.c

test_expr: test_expr.o storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o expr.o record_mgr.o rm_serializer.o btree_mgr.o
	gcc -o test_expr_Output test_expr.o storage_mgr.o dberror.o buffer_mgr.o buffer_mgr_stat.o expr.o record_mgr.o rm_serializer.o btree_mgr.o

test_expr.o: test_expr.c storage_mgr.h dberror.h buffer_mgr.h buffer_mgr_stat.h expr.h record_mgr.h btree_mgr.h
	gcc -c test_expr.c

btree_mgr.o: btree_mgr.c btree_mgr.h
	gcc -c btree_mgr.c

storage_mgr.o: storage_mgr.c storage_mgr.h
	gcc -c storage_mgr.c

dberror.o: dberror.c dberror.h
	gcc -c dberror.c

buffer_mgr.o: buffer_mgr.c buffer_mgr.h
	gcc -c buffer_mgr.c

buffer_mgr_stat.o: buffer_mgr_stat.c buffer_mgr_stat.h
	gcc -c buffer_mgr_stat.c

expr.o: expr.c expr.h
	gcc -c expr.c

record_mgr.o: record_mgr.c record_mgr.h
	gcc -c record_mgr.c

rm_serializer.o: rm_serializer.c
	gcc -c rm_serializer.c

clean: 
	rm testOutput test_expr_Output *.o *.bin