#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char ch;
	FILE* fp=popen("tail $a /var/log/kern.log","r");
	while((ch=fgetc(fp))!=EOF)    //不到文件结束就循环
            putchar(ch);          //输出一个字符
    pclose(fp); 
    return 0;
}
