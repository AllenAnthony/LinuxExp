#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<unistd.h>

#include<sys/types.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<sys/wait.h>

int main()
{
	puts("I am main process");  //p0
	int p0=getpid();
	printf("pid of P0 is %d\r\n",p0);

	int fd1[2];
	int fd2[2];

	if(pipe(fd1)==-1 | pipe(fd2)==-1)
	{
		printf("create pipe fail\r\n");
		return 0;
	}

	int p1=fork();  //p1

	if(p1<0)//进程创建失败
	{
		printf("p1进程创建失败");
		return 0;

	}
	else if(p1>0)//p0
	{
		printf("pid of P1 is %d\r\n",p1);
		int p2=fork(); //p2
		if(p2<0)//进程创建失败
	    {
			printf("p2进程创建失败");
			return 0;

		}
		else if(p2>0) //p0
		{
			printf("pid of P2 is %d\r\n",p2);
			waitpid(p2,NULL,0);
		}
		else //p2
		{
			
			close(fd1[1]);
			close(fd2[0]);

			char *str2="Child process p2 is sending a message!";
			write(fd2[1],str2,strlen(str2));
			close(fd2[1]);
			char buf[100];
			read(fd1[0],buf,100);

			puts(buf);
		}

		waitpid(p1,NULL,0);

	}
	else //p1
	{
		int p3=fork(); //p3
		if(p3<0)//进程创建失败
	    {
			printf("p3进程创建失败");
			return 0;

		}
		else if(p3>0) //p1
		{
			printf("pid of P3 is %d\r\n",p3);
			close(fd1[0]);
			close(fd2[1]);

			char *str1="Child process p1 is sending a message!";
			write(fd1[1],str1,strlen(str1));
			close(fd1[1]);
			char buf[100];
			read(fd2[0],buf,100);

			puts(buf);

			waitpid(p3,NULL,0);
		}
		else //p3
		{
			
			puts("I am child process p3");

			if(execl("/bin/ls","ls","-al",NULL)!=-1)
				puts("execl 执行成功");

		}

	}



}

