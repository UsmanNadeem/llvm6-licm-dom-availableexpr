// taken from wikipedia
#include <stdio.h>
#include <stdlib.h>
int a[100] = {3};

int test(int x){
	int y = 5;
	int z = 8;
	for (int i = 0; i < 100; ++i) {
	    x = y + z;
	    a[i] = 6 * i + x * x;
	}
	return x;
}

int main(){
	// a = malloc(sizeof(int) * 100);
	int r = test(100);
	printf("result is: %d\n",r);
	return 0;
}