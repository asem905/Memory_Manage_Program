/*
Author:Assem Samy
File:app.c
comment:contains main application
*/
#include "memory_manage.h"
int main(){
	/*just an example for you to test by*/
	char *ptr=(char*)HmmAlloc(1024);
	printf("block address is %p as we added size of metadata at start before block address by size of metadata it self which is %d\n",ptr,sizeof(blockheader_t));
	printf("\n");
	char *ptr2=(char*)HmmAlloc(1024);
	printf("\n");
	char *ptr3=(char*)HmmAlloc(1024);
	printf("\n");
	char *ptr5=(char*)HmmAlloc(125*1024);
	printf("\n");
	char *ptr4=(char*)HmmAlloc(2*1024);
	printf("\n");
	char *ptr6=(char*)HmmAlloc(3*1024);
	printf("\n");
	char *ptr7=(char*)HmmAlloc(4*1024);
	printf("\n");
	HmmFree(ptr7);
	printf("\n");
	HmmFree(ptr5);
	printf("\n");
	HmmFree(ptr6);
	char *ptr8=(char*)HmmAlloc(1024);
	return 0;
}


