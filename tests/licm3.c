#include <stdio.h>
int main(){
	double B[ 100 ] [ 100 ] = {13} ;
	double A[ 100 ] = {1} ;
	int s = 0;
	for (int i =0; i <100; i++) {
		for (int j =0; j <10; j++) {
			for (int k=0; k<100; k++) {
				s += B[ i ] [ k ] * B[ k ] [ i ] ;
				s += B[ j ] [ k ] * B[ k ] [ j ] ;
				s += A[ i ] * B[ i ] [ i ] ;
			}
		}
	}
printf("result is: %d\n",s);
return 0;
}
