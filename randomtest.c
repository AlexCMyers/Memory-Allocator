#include <stdlib.h>
#include <stdio.h>
#include <time.h>
int main(int argc, char* argv[]){
	char* megaPtrs[100];
	
	int i = 0;
	int x;
	int y;
	int j;
	srand(atoi(argv[1]));
	
	for(i = 0; i < 100; i++){
		megaPtrs[i] = NULL;
	}
	
	
//	for(i = 0; i < 1000; i++){
	for(i = 0; i < 10000; i++){
		x = rand()%100;
	//	printf("%d %d\n", i, x);
		if(megaPtrs[x]){
			y = rand()%2;
//			if(x == 9){printf("%d ",y);} 
			if(y == 0){
				free(megaPtrs[x]);
				megaPtrs[x] = NULL;
			}else if(y == 1){
				y = rand()%3;
		//		if(x == 9){printf("%d \n",y);} 
				if(y == 0){y = (rand()%16) + 1;}
				else if(y==1){y = (rand()%250) + 16;}
				else{y = (rand()%5000) + 256;}
//				if(x == 9){printf("%d \n",y);} 
				
				megaPtrs[x] = (char*)realloc(megaPtrs[x], y*sizeof(char));
				for(j = 0; j < y; j++){
					//memory should retain it's original values
					//if new memory is added, then should be 0
					if(megaPtrs[x][j] != j%128 && megaPtrs[x][j] != 0){
						printf("boo j = %d and the char is %c\n", j, megaPtrs[x][j]);
						exit(1);
					}
				}
			}
		}
		else{
			y = rand()%3;
			if(y == 0){y = (rand()%16) + 1;}
			else if(y==1){y = (rand()%250) + 16;}
			else{y = (rand()%5000) + 256;}
//			printf("Memory allocated: %d \n",y); 
			megaPtrs[x] = (char*)malloc(sizeof(char) *y);
			for(j = 0; j < y; j++){
				*(megaPtrs[x]+ j) = j%128;
			}
		}
	}



	return 0;
}
