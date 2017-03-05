#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <ctype.h>
#include <dirent.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include <time.h>
#include<sys/wait.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "Macros.h"


///////////////////////////////////////////   环境变量    /////////////////////////////////////////////////
char COMP[PROMPT_LENGTH];       //命令提示符
char ACOMP[PROMPT_LENGTH];
char PWD[MAX_PATH];             //当前工作目录                   
char OLD_PWD[MAX_PATH];
char USER[NAME_LENGTH];         //用户信息
char HOST[NAME_LENGTH];
char HOME[MAX_PATH];
char SHELL[MAX_PATH];
char PATH[MAX_PATH];

struct treenode* TREE;         //全局搜索树
struct ProArray * processqueue;              //进程队列

struct variable
{
	char name[NAME_LENGTH];                         //变量名
	char value[VAR_LENGTH];                         //变量值
};

struct variable* var[VAR_MAX];
int varnum;

struct variable* GP[VAR_MAX];           //全局参数
int GPnum;                              //全局参数个数



enum PROCESS_STATE
{
	DONE = 1,
	RUNNING = 2,
	STOP = 3,
};


struct Process                       //进程结构
{
	enum PROCESS_STATE state;
	char name[INPUT_MAX];
	int number;
	int pid;
};

struct ProArray                      //进程数组，用来处理进程
{
	struct Process Data[PROCESS];
	int head;
	int tail;
	int size;
};


struct ListNode                      //链表节点
{
	WORD content;
	char path[NAME_LENGTH];
	struct ListNode* next;
};


struct List                           //链表
{
	struct ListNode* head;
	struct ListNode* tail;
	int number;
};


struct treenode{                         //搜索树
	WORD word[5];                    
	char path[NAME_LENGTH];                               
	struct treenode* next[NODE_SIZE];
	char letter;
	int isword;
	int number;
};
typedef struct ListNode Node;








struct ProArray* newarray();//创建新的进程数组
void deleteArr(char* array, int len, int pos);//从数组中删除
void insertArr(char* array, int len, int pos, char ch);//在数组中插入
int quit();//指令退出程序
int myshell(int argc, char const * argv[]);// 用shell来解释一个shell脚本
int set(int argc, char const * argv[]);//指令用来设置变量的值
int unset(int argc, char const * argv[]);
void shift(int n);//指令shift,用来左移全局参数
void printhistory();//指令history，用来打印历史指令信息
void assign(char *left, char * right);//指令assign，用来传值
void printerror(int errorNum);//错误输出
void  printprocess();//指令打印进程信息
void absolutePath(char * path);//把相对路径转换成绝对路径
void UPDATE();//更新命令提示符和当前工作目录的状态以及某些环境变量
int cd(char * path);//指令cd
int clr();//指令clr，用来清屏
int dir(char * path);//指令dir,列出该目录下的所有文件
int environ();//指令environ，用来列出所有环境变量
int echo(int argc, char const * argv[]);
int help();//指令help，给出指令介绍
void treeinsert(WORD word, char* path, int script);//在树中插入节点
int isexist(char* word);             //检测word是否在树中
void searchtree(struct treenode * Tree, char *Word, struct List *mylist);  //在树中搜索是否存在Word
void dealVar(int argc, char* argv[PARAMETER]);//处理变量
void RUN(int argc, char *argv[PARAMETER]);//执行指令
void mytime();
void devideInput(char *Input);//分割输入的字符串
void analyInput(int argc, char *argv[PARAMETER]);//分析分割后的输入字符串
char getch();
int clearLine(int n);
struct treenode * newnode();//创建新的搜索树节点
void insert(struct List* mylist, char *path, WORD word);//链表插入节点
int findvar(char *myvar);//在var中检测是否存在myvar
char * getvariable(char * myvar);// 返回myvar对应的变量的值
void updatevar(char* myvar, char* var_value);// 如果变量名对应变量已经存在则更新，否则添加新的变量
void initialvar();//初始化所有变量
void ouputCommand(char buf[BUF_LENGTH], int argc, char *const argv[]);// 执行输出，执行结果存储在GP[0]
void inputCommand(char buf[BUF_LENGTH], int argc, char *const argv[]);//执行输入
struct Process deletePA(struct ProArray * PA);//删除进程
void insertPA(struct ProArray * PA, struct Process item);//插入进程
struct List* newlist();//创建新的链表
void Initial(char * ShellPath);//主要环境变量包括{ "HOME", "COMP", "ACOMP", "SHELL","HOST", "USER", "PATH", "PWD" }
void env_tree(char* path);// 把环境变量PATH中所有目录下的可执行程序都放入树
void dealInput(char *Input);

#endif