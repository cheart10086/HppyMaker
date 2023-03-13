#include <stdio.h>

void fun(int *a,int *b)
{
	*a=3;
	*b=2;
} 

int main()
{
    int *a,*b;
    fun(&a,&b);
	
    printf("%d %d",&a,&b);
}