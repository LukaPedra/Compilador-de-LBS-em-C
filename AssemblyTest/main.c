#include <stdio.h>

unsigned char aa[3];

void testeSizeof(unsigned char *commands, size_t bytes){
	printf("%d\n",bytes);
}
//int function2(int x);
int main(void){
	printf("%d\n",sizeof(aa));
	testeSizeof(aa,sizeof(aa));
	//int a;
	//a = function2(3);
	//printf("%d\n",a);
}