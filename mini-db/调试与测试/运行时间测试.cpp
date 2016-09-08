#include<stdio.h>
#include<time.h>

double t1,t2;
int i=0; 

int main(){
	
	t1 = clock();
	for(i=0;i<100000;i++);
	t2 = clock();
	
	printf("t1:%.5lfms  t2:%.5lfms  time:%.5lfms\n",t1,t2,t2-t1);
	
	//10^5    O(1)            ---->    0ms 
	//10^7 （一百万）O(1)     ---->    50ms
	//2*10^8 （两千万）O(1)   ---->    990ms~1000ms 
}
