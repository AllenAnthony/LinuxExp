#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<unistd.h>

#include<sys/types.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<sys/wait.h>

#include<pthread.h>

int* array;

void* function(void* arg)
{
	int N=*((int*)arg);
	free(array);
	array=(int*)malloc(N*sizeof(int*));

	array[0]=0;
	array[1]=1;

	for(int i=2;i<N;i++)
	{
		array[i]=array[i-1]+array[i-2];
	}

	return NULL;
}

int main()
{
	pthread_t ptid;
	puts("please input the length of the fibonacci sequence: ");
	int N;
	scanf("%d",&N);

	int result;
	result=pthread_create(&ptid,NULL,function,&N);
	if(result)
	{
		puts("error in function");
		return 1;
	}

	result=pthread_join(ptid,NULL);
	if(result)
	{
		puts("error in join");
		return 1;
	}

	printf("the result is: \r\n");
	for(int i=0;i<N;i++)
		printf("%d\r\n",array[i]);
	
	return 0;
}

