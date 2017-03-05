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


///////////////////////////////////////////   ��������    /////////////////////////////////////////////////
char COMP[PROMPT_LENGTH];       //������ʾ��
char ACOMP[PROMPT_LENGTH];
char PWD[MAX_PATH];             //��ǰ����Ŀ¼                   
char OLD_PWD[MAX_PATH];
char USER[NAME_LENGTH];         //�û���Ϣ
char HOST[NAME_LENGTH];
char HOME[MAX_PATH];
char SHELL[MAX_PATH];
char PATH[MAX_PATH];

struct treenode* TREE;         //ȫ��������
struct ProArray * processqueue;              //���̶���

struct variable
{
	char name[NAME_LENGTH];                         //������
	char value[VAR_LENGTH];                         //����ֵ
};

struct variable* var[VAR_MAX];
int varnum;

struct variable* GP[VAR_MAX];           //ȫ�ֲ���
int GPnum;                              //ȫ�ֲ�������



enum PROCESS_STATE
{
	DONE = 1,
	RUNNING = 2,
	STOP = 3,
};


struct Process                       //���̽ṹ
{
	enum PROCESS_STATE state;
	char name[INPUT_MAX];
	int number;
	int pid;
};

struct ProArray                      //�������飬�����������
{
	struct Process Data[PROCESS];
	int head;
	int tail;
	int size;
};


struct ListNode                      //����ڵ�
{
	WORD content;
	char path[NAME_LENGTH];
	struct ListNode* next;
};


struct List                           //����
{
	struct ListNode* head;
	struct ListNode* tail;
	int number;
};


struct treenode{                         //������
	WORD word[5];                    
	char path[NAME_LENGTH];                               
	struct treenode* next[NODE_SIZE];
	char letter;
	int isword;
	int number;
};
typedef struct ListNode Node;








struct ProArray* newarray();//�����µĽ�������
void deleteArr(char* array, int len, int pos);//��������ɾ��
void insertArr(char* array, int len, int pos, char ch);//�������в���
int quit();//ָ���˳�����
int myshell(int argc, char const * argv[]);// ��shell������һ��shell�ű�
int set(int argc, char const * argv[]);//ָ���������ñ�����ֵ
int unset(int argc, char const * argv[]);
void shift(int n);//ָ��shift,��������ȫ�ֲ���
void printhistory();//ָ��history��������ӡ��ʷָ����Ϣ
void assign(char *left, char * right);//ָ��assign��������ֵ
void printerror(int errorNum);//�������
void  printprocess();//ָ���ӡ������Ϣ
void absolutePath(char * path);//�����·��ת���ɾ���·��
void UPDATE();//����������ʾ���͵�ǰ����Ŀ¼��״̬�Լ�ĳЩ��������
int cd(char * path);//ָ��cd
int clr();//ָ��clr����������
int dir(char * path);//ָ��dir,�г���Ŀ¼�µ������ļ�
int environ();//ָ��environ�������г����л�������
int echo(int argc, char const * argv[]);
int help();//ָ��help������ָ�����
void treeinsert(WORD word, char* path, int script);//�����в���ڵ�
int isexist(char* word);             //���word�Ƿ�������
void searchtree(struct treenode * Tree, char *Word, struct List *mylist);  //�����������Ƿ����Word
void dealVar(int argc, char* argv[PARAMETER]);//�������
void RUN(int argc, char *argv[PARAMETER]);//ִ��ָ��
void mytime();
void devideInput(char *Input);//�ָ�������ַ���
void analyInput(int argc, char *argv[PARAMETER]);//�����ָ��������ַ���
char getch();
int clearLine(int n);
struct treenode * newnode();//�����µ��������ڵ�
void insert(struct List* mylist, char *path, WORD word);//�������ڵ�
int findvar(char *myvar);//��var�м���Ƿ����myvar
char * getvariable(char * myvar);// ����myvar��Ӧ�ı�����ֵ
void updatevar(char* myvar, char* var_value);// �����������Ӧ�����Ѿ���������£���������µı���
void initialvar();//��ʼ�����б���
void ouputCommand(char buf[BUF_LENGTH], int argc, char *const argv[]);// ִ�������ִ�н���洢��GP[0]
void inputCommand(char buf[BUF_LENGTH], int argc, char *const argv[]);//ִ������
struct Process deletePA(struct ProArray * PA);//ɾ������
void insertPA(struct ProArray * PA, struct Process item);//�������
struct List* newlist();//�����µ�����
void Initial(char * ShellPath);//��Ҫ������������{ "HOME", "COMP", "ACOMP", "SHELL","HOST", "USER", "PATH", "PWD" }
void env_tree(char* path);// �ѻ�������PATH������Ŀ¼�µĿ�ִ�г��򶼷�����
void dealInput(char *Input);

#endif