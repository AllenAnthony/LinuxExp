#include "myshell.h"
char* INS[] = { "cd", "clr", "dir", "environ", "echo", "help", "quit", "myshell", "set", "time", "shift","history","other" };//指令集合

void absolutePath(char * path)//把相对路径转换成绝对路径
{
	char realPath[MAX_PATH];
	if (*path == '~')
	{
		strcpy(realPath, HOME);
		strcat(realPath, (path + 1));
	}
	else if (*path == '.')
	{
		strcpy(realPath, PWD);
		strcat(realPath, (path + 1));
	}
	strcpy(path, realPath);
}

void UPDATE()//更新命令提示符和当前工作目录的状态以及某些环境变量
{
	char wd[MAX_PATH];
	getcwd(wd, MAX_PATH);                   //获取当前的工作目录到wd
	strcpy(OLD_PWD, PWD);
	updatevar("OLD_PWD", OLD_PWD);
	strcpy(PWD, wd);
	updatevar("PWD", PWD);


	if (strstr(wd, HOME))
	{
		strcpy(wd, "~");
		strcat(wd, &(wd[strlen(HOME)]));
	}
	sprintf(COMP, "newshell::%s@%s:%s$", USER, HOST, wd);
	updatevar("COMP", COMP);
}


void dealInput(char *Input)
{
	int cou;
	int length = strlen(Input);
	struct dirent * dirP;
	DIR * dirSS = opendir(PWD);
	char namestr[NAME_LENGTH];
	for (cou = 0; cou<length; cou++)
	{
		if (Input[cou] == ' ' && (cou + 1<length))
			Input = Input + cou + 1;
	}
	length = strlen(Input);

	while (dirP = readdir(dirSS))
	{
		strcpy(namestr, dirP->d_name);
		if (strlen(namestr) >= length)
		{
			namestr[length] = '\0';
			if (!strcmp(namestr, Input))
			{
				strcpy(Input, dirP->d_name);
				for (cou = length; cou>0; cou--)
					printf("\b \b");

				puts(Input);
				return;
			}
		}
	}
	strcpy(namestr, Input);
	strcat(namestr, "*");

	struct List* pList = (struct List*)newlist();

	searchtree(TREE, namestr, pList);
	struct ListNode * pNode = pList->head;
	if (pNode)
	{
		printf("\n");
		int i = 0;
		while (pNode)
		{
			printf("%s      ", pNode->content);
			i += 1;
			if (i == 5)
			{
				printf("\n");
				i = 0;
			}
			pNode = pNode->next;
		}
		printf("\n%s ", COMP);
		puts(Input);
	}
}

void dealVar(int argc, char* argv[PARAMETER]) //处理变量
{
	int i, j;
	int flag;
	for (i = 0; i<argc; i++)
	{
		char * arguement = (char*)malloc(256);
		char * new_argv = (char*)malloc(256);
		strcpy(arguement, argv[i]);
		flag = 3;
		if (arguement == NULL)
			continue;

		for (j = 0; j<strlen(arguement); j++)
		{
			if (arguement[j] == '\'' || arguement[j] == '\"')
			{
				if (j>0 && arguement[j - 1] == '\\')
				{
					continue;
				}
				if (flag == 1 || flag == 2)
					break;
				arguement++;
				break;
			}
		}
		char * myvar;
		if (flag == 1 || flag == 2)
		{
			if (arguement && *arguement == '$')
			{
				for (j = j + 1; j<strlen(arguement); j++)
				{
					if (arguement[j] == '\"' || arguement[j] == '\'')
						arguement[j] = '\0';
				}

				if (flag == 1)
				{
					myvar = arguement + 1;
					strcpy(new_argv, getvariable(myvar));
				}
				else
				{
					myvar = arguement;
					strcpy(new_argv, myvar);
				}
				strcat(new_argv, arguement + j + 1);
			}
			else
			{
				for (j = 0; j<strlen(arguement); j++)
				{
					if (arguement[j] == '\"' || arguement[j] == '\'')
					{
						arguement[j] = '\0';
					}
				}
				strcpy(new_argv, arguement);
				strcat(new_argv, arguement + j + 1);
			}
		}
		else
		{
			if (arguement && *arguement == '$')
			{
				char *myvar = arguement + 1;
				strcpy(new_argv, getvariable(myvar));
			}
			else
			{
				strcpy(new_argv, arguement);
			}
		}
		argv[i] = new_argv;
	}
}

int findvar(char *myvar)//在var中检测是否存在myvar
{                   
	for (int i = 1; var[i] != NULL; i++)
	{
		if (!strcmp(var[i]->name, myvar))
			return i;	
	}
	return 0;                               
}

char * getvariable(char * myvar) // 返回myvar对应的变量的值
{                
	if (!myvar) 
		return "";
	else if (*myvar >= '0' && *myvar <= '9' && GP[*myvar - '0'] && GP[*myvar - '0']->value)
		return (GP[*myvar - '0']->value);
	else if ((*myvar == '?') && GP[0] && GP[0]->value)
		return GP[0]->value;
	else if (*myvar == '#')
	{
		char * arguement = (char*)malloc(4);
		sprintf(arguement, "%d", GPnum);
		if (arguement)
			return arguement;
	}
	else if (*myvar == '*' || *myvar == '@')
	{      
		char * arguement = (char* )malloc(NAME_LENGTH);
		memset(arguement, 0, sizeof(arguement));
		for (int i = 1; i <= GPnum; i++)
		{
			strcat(arguement, GP[i]->value);
			strcat(arguement, " ");
		}
		if (arguement)
			return arguement;
	}
	else
	{
		int res = findvar(myvar);
		if (var[res] && var[res]->value)
			return (var[res]->value);
	}
	return "";
}




int clr()//指令clr，用来清屏
{
	printf("\033c");
	return 0;
}

int dir(char * path)//指令dir,列出该目录下的所有文件
{
	if (path != NULL && *path == '~')//若有～，转换成实际地址
	{                                   
		char tempath[MAX_PATH];
		strcpy(tempath, HOME);
		strcat(tempath, &(path[1]));
		path = tempath;
	}

	char * canshu[128] = { "ls", path };  

	pid_t pid = fork();
	if (!pid)
	{
		execvp("ls", canshu);                     //执行 ls path
		printerror(errno);
	}
	else
		wait(NULL);
	return 0;
}

int environ()//指令environ，用来列出所有环境变量
{
	for (int i = 0; i<varnum; i++)
	{
		printf("%s=%s\n", var[i]->name, var[i]->value);
	}
	return 0;
}

void Initial(char * ShellPath)
{

	updatevar("NULL", "");
	char path[MAX_PATH];
	gethostname(HOST, NAME_LENGTH);updatevar("HOST", HOST);
	strcpy(USER, getenv("USER"));updatevar("USER", USER);
	strcpy(PATH, getenv("PATH"));updatevar("PATH", PATH);
	strcpy(PWD, getenv("PWD"));updatevar("PWD", PWD);
	strcpy(HOME, getenv("HOME"));updatevar("HOME", HOME);
	strcpy(path, ShellPath);
	absolutePath(path);
	strcpy(SHELL, path);
	updatevar("SHELL", SHELL);
	updatevar("OLD_PWD", PWD);
	updatevar("COMP", "");
	updatevar("PS2", ">");

	UPDATE();

	int i;
	char *path_ptr;
	struct dirent * dir_ptr;
	DIR * dir_stream;

	strcpy(path, PATH);
	path_ptr = path;
	TREE = (struct Node*)newnode();
	for (i = 0; i<strlen(PATH); i++)
	{
		if (path[i] == ':')
		{
			path[i] = '\0';
			dir_stream = opendir(path_ptr);
			while (dir_ptr = readdir(dir_stream))
			{
				if (isalpha(*(dir_ptr->d_name)) && dir_ptr->d_type == DT_REG)
				{
					WORD word;
					strcpy(word, dir_ptr->d_name);
					treeinsert(word, path_ptr, 0);
				}
			}
			closedir(dir_stream);
			path_ptr = (path + i + 1);
		}
	}


	for (i = 0; i<sizeof(INS) / sizeof(char*); i++)
	{
		WORD word;
		strcpy(word, INS[i]);
		//treeinsert(word, NULL, 0);
	}
	initialvar();
	processqueue = newarray();
}


int echo(int argc, char const * argv[])
{
	for (int i = 1; i<argc; i++)
	{                    
		printf("%s ", argv[i]);
	}
	printf("\n");
}

void  printprocess()//指令打印进程信息
{
	struct Process proc;
	int i, j;
	for (j = 0, i = processqueue->head; i <= processqueue->tail; j++, i++)
	{
		printf("[%d]\t\t\t%s\n", processqueue->Data[i].pid, processqueue->Data[i].name);
	}

	if (DONE == processqueue->Data[processqueue->head].state)
	{
		deletePA(processqueue);
	}
}



void mytime()
{
	time_t timep; time(&timep);
	printf("%s", asctime(gmtime(&timep)));
}
void RUN(int argc, char *argv[PARAMETER])//执行指令
{
	char* order = argv[0];
	for (int i = 0; i<sizeof(INS) / sizeof(char*); i++)
	{
		if (!strcmp(order, INS[i]))
		{
			switch (i)
			{
			case 0: cd(argv[1]);
				break;
			case 1: clr();
				break;
			case 2: dir(argv[1]);
				break;
			case 3: environ();
				break;
			case 4: echo(argc, argv);
				break;
			case 5: help();
				break;
			case 6: quit();
				break;
			case 7: myshell(argc, argv);
				break;
			case 8: set(argc, argv);
				break;
			case 9: mytime();
				break;
			case 10:shift(argc);
				break;
			case 11:printhistory(argc, argv);
				break;
			case 12:unset(argc, argv);
				break;
			case 13:;
				break;
			}
			return;
		}
	}

	if (*order == '~' || *order == '/' || *order == '.')
	{
		argv[0] = order;
		absolutePath(order);
		execv(order, argv);
		printf("%s: ", order);
		printerror(errno);
		return;
	}
	pid_t pid = fork();
	if (pid == 0)
	{
		struct List *pList = (struct List*)newlist();
		execvp(order, argv);
		strcpy(GP[0]->value, "1");

		char namestr[256];
		strcpy(namestr, "?");
		strcat(namestr, order);
		int cou = 0;
		while (cou <= strlen(order) && cou<NAME_LENGTH && strlen(order)>1)
		{
			strcpy(namestr, order);
			namestr[cou] = '?';
			searchtree(TREE, namestr, pList);
			cou++;
		}
		struct ListNode * pNode = pList->head;
		printf("No order '%s' found", order);
		if (pNode)
		{
			printf(", did you mean:\n");
			while (pNode)
			{
				printf("Command %s from %s\n", pNode->content, pNode->path);
				pNode = pNode->next;
			}
		}
		else
			puts("\n");
		quit();
	}
	else
		wait(NULL);
}


void updatevar(char* myvar, char* var_value)// 如果变量名对应变量已经存在则更新，否则添加新的变量
{
	if (findvar(myvar))
	{
		strcpy(var[findvar(myvar)]->value, var_value);
	}
	else if (varnum<1024)//找不到
	{
		var[varnum] = (struct variable*)malloc(sizeof(struct variable));


		strcpy(var[varnum]->name, myvar);
		strcpy(var[varnum]->value, var_value);
		varnum++;
	}
}

void initialvar()//初始化所有变量
{
	for (int i = 0; i<VAR_MAX; i++)
	{
		GP[i] = (struct variable*)malloc(sizeof(struct variable));
		strcpy(GP[i]->name, "");
		strcpy(GP[i]->value, "");
	}

	strcpy(GP[0]->value, "0");
	GPnum = 1;
}


void ouputCommand(char buf[BUF_LENGTH], int argc, char *const argv[])   // 执行输出，执行结果存储在GP[0]
{
	pid_t pid;
	int fd[2];

	memset(buf, 0, BUF_LENGTH);
	int *read_fd = &fd[0];
	int *write_fd = &fd[1];

	pipe(fd);
	initialvar();
	pid = fork();
	if (pid == 0)//子进程
	{
		close(*read_fd);
		dup2(*write_fd, STDOUT_FILENO);                  //标准输出重定向到write_fd	
		close(*write_fd);                               // 把write_fd关掉标准输出仍能写入管道              
		RUN(argc, argv);
		exit(1);
	}
	else//父进程
	{
		close(*write_fd);
		wait(NULL);
		read(*read_fd, buf, BUF_LENGTH);
		close(*read_fd);
	}
}
int help()//指令help，给出指令介绍
{
	char helpout[] ="COMMAND              INTRODUCTION                                               \n\
cd                   change current dictionary                                  \n\
clr                  clear the screen                                           \n\
dir                  all files in the directory are listed                      \n\
environ              all environment variables in the directory arelisted listed\n\
echo                 output something                                           \n\
help                 ouput the introduction of all the instructions             \n\
time                 show the time of the system                                \n\
quit                 exit from the system                                       \n\
myshell(filename)   excute the shell program                                    \n\
set[command]        set a varible to some value                                 \n\
history              output all the instruction in history                      \n\
					                                                                                  ";
	puts(helpout);
}

int quit()//指令退出程序
{
	exit(0);
}

int myshell(int argc, char const * argv[])// 用shell来解释一个shell脚本
{	
	char path[MAX_PATH];
	char tmpbuf[BUF_LENGTH];
	char* tmp;
	char zhushi[MAX_PATH];
	FILE * fp;

	if (argc == 1)
	{
		fgets(path, MAX_PATH, stdin);
	}
	else
	{
		strcpy(path, argv[1]);
	}
	
	if ((fp = fopen(path, "r")) == NULL)
	{
		printerror(errno);
		return 1;
	}

	fgets(tmpbuf, BUF_LENGTH, fp);
	if ((tmp = strstr(tmpbuf,"#!")) != NULL)
	{
		strcpy(zhushi, tmpbuf + 2);
		fgets(tmpbuf, BUF_LENGTH, fp);
	}

	while (1)
	{
		tmpbuf[strlen(tmpbuf) - 1] = '\0';                  //去掉换行符
		if (strlen(tmpbuf)>0)
		{
			if ((tmp = strchr(tmpbuf, '#')) != NULL)
				*tmp = '\0';
			devideInput(tmpbuf);
		}
		if (feof(fp))
			break;
		
		fgets(tmpbuf, BUF_LENGTH, fp);
	}

	return 0;
}

int set(int argc, char const * argv[])//指令用来设置变量的值
{
    if (argc > 1)
	{                                                
		char buf[BUF_LENGTH];
		ouputCommand(buf, argc - 1, &(argv[1]));
		if (strcmp(GP[0]->value, "0") == 0)
		{                                   
			int i;
			char * ptr = buf;
			for (i = 0; i<strlen(buf); i++)
			{                          
				if (buf[i] == ' ')
				{
					buf[i] = '\0';
					strcpy(GP[GPnum++]->value, ptr);                
					ptr++;
				}
			}
			strcpy(GP[GPnum++]->value, ptr);
		}
	}
	return 0;
}

int unset(int argc, char const * argv[])
{
	for (int i = 1; i<argc; i++)
	{
		int res = findvar(argv[i]);
		strcpy(var[res]->value, "");
	}
}


void shift(int n)//指令shift,用来左移全局参数
{
	int i;
	for (i = 1; i<GPnum; i++)
	{
		GP[i] = GP[(i + n) % GPnum];
	}
	for (; i<VAR_MAX; i++)
	{
		strcpy(GP[i]->value, "");
	}
}

void printhistory()//指令history，用来打印历史指令信息
{
	for (int i = 0; i<historysize; i++)
	{
		printf("%s\n",history[i]);
	}
}

void assign(char *left, char * right)//指令assign，用来传值
{
	while (*right == ' ')
		right++;
	while (*left == ' ')
		left++;

	char *temp[128];
	memset(temp, 0, sizeof(char*)*128);
	temp[0] = right;
	dealVar(1, temp);                  
	right = temp[0];
	updatevar(left, right);
}

void printerror(int errorNum){}  //错误输出
	
void devideInput(char *Input)//分割输入的字符串
{
	char* command;
	char *canshu[PARAMETER];

	while (*Input == ' ')//清除前面的空格
		Input++;

	int length = strlen(Input);
	if (!length)
		return;

	int Pnum = 1;
	memset(canshu, 0, sizeof(canshu));
	command = Input;
	canshu[0] = Input;
	for (int i = 1; i<length; i++)
	{
		if (Input[i] == ' ')
		{
			Input[i] = '\0';
			i++;
			while (Input[i] == ' ')
			{
				Input[i] = '\0';
				i++;
			}
			i--;
			canshu[Pnum++] = &(Input[i + 1]);
		}
		else if (Input[i] == '=')
		{
			Input[i] = '\0';
			assign(Input, (Input + i + 1));
			return;
		}
	}
	dealVar(Pnum, canshu);
	analyInput(Pnum, canshu);
}

void analyInput(int argc, char *argv[PARAMETER])//分析分割后的输入字符串
{
	int myargc;
	int i;
	char * myargv[PARAMETER];
	memset(myargv, 0, sizeof(char*)*PARAMETER);

	char buf[BUF_LENGTH];

	FILE * fp;
	char tempfile[] = "tempfile-XXXXXX";
	int tempfd = mkstemp(tempfile);

	for (i = 0, myargc = 0; i<argc && argv[i]; i++)
	{
		if (*(argv[i]) == '&')//后台执行命令
		{
			int pid = vfork();
			int num = processqueue->tail + 1;
			if (pid == 0)
			{
				struct Process newProc;
				for (int k = 0; k<myargc; k++)
					strcat(newProc.name, myargv[k]);

				newProc.number = num + 1;
				newProc.pid = pid;
				newProc.state = RUNNING;
				insertPA(processqueue, newProc);
				printf("[%d] %d\n", newProc.number, newProc.pid);
				quit();
			}
			else
			{
				if (fork() == 0)
				{
					execvp(myargv[0], myargv);
					printerror(errno);
					exit(0);
				}
				processqueue->Data[num].state = DONE;
			}
			memset(myargv, 0, sizeof(char*)*PARAMETER);
			myargc = 0;
		}
		else
			myargv[myargc++] = argv[i];

	}

	remove(tempfile);
	if (myargc == i)
		RUN(argc, argv);
}

char getch()
{
	char tmpbuf = 0;
	struct termios orig = { 0 };
	fflush(stdout);

	if (tcgetattr(0, &orig)<0)
		printerror(errno);

	orig.c_lflag &= ~ICANON;
	orig.c_lflag &= ~ECHO;
	orig.c_cc[VMIN] = 1;
	orig.c_cc[VTIME] = 0;

	if (tcsetattr(0, TCSANOW, &orig)<0)
		printerror(errno);

	if (read(0, &tmpbuf, 1)<0)
		printerror(errno);

	orig.c_lflag |= ICANON;
	orig.c_lflag |= ECHO;

	if (tcsetattr(0, TCSADRAIN, &orig)<0)
		printerror(errno);

	return tmpbuf;
}

int clearLine(int n)
{
	printf("\r");
	//printf("\n%s ", COMP);
	int i;
	for (i = n; i>0; i--) printf(" ");
	for (i = n; i>0; i--) printf("\b");
	return n;
}
void Input_Completion(char *myinput)
{
	int cou;
	int length = strlen(myinput);
	struct dirent * dirP;
	DIR * dirSS = opendir(PWD);
	char namestr[NAME_LENGTH];
	for (cou = 0; cou<length; cou++)
	{
		if (myinput[cou] == ' ' && (cou + 1<length))
			myinput = myinput + cou + 1;
	}
	length = strlen(myinput);

	while (dirP = readdir(dirSS))
	{
		strcpy(namestr, dirP->d_name);
		if (strlen(namestr) >= length)
		{
			namestr[length] = '\0';
			if (!strcmp(namestr, myinput))
			{
				strcpy(myinput, dirP->d_name);
				for (cou = length; cou>0; cou--)
					printf("\b \b");

				puts(myinput);
				return;
			}
		}
	}
	strcpy(namestr, myinput);
	strcat(namestr, "*");

	struct List* pList = (struct List*)newlist();

	searchtree(TREE, namestr, pList);
	struct ListNode * pNode = pList->head;
	if (pNode)
	{
		printf("\n");
		int i = 0;
		while (pNode)
		{
			printf("%s      ", pNode->content);
			i += 1;
			if (i == 5)
			{
				printf("\n");
				i = 0;
			}
			pNode = pNode->next;
		}
		printf("\n%s ", COMP);
		puts(myinput);
	}
}

int main(int argc, char *argv[])
{
	Initial(argv[0]);
	historysize = 0;
	historyP = 0;

	char *args[MAXLINE / 2 + 1];
	char Input[INPUT_MAX];
	memset(Input, 0, sizeof(Input));
	memset(history, 0, sizeof(history));

	while (1)
	{
		puts(COMP);
		fflush(stdout);
		unsigned char ch;

		memset(Input, 0, sizeof(Input));
		int index = 0;
		int length = 0;

		do
		{
			clearLine(length + 1);
			length = printf("%s", Input);
			for (int k = index; k<length; k++)
				printf("\b");


			ch = getch();

			if (isprint(ch))
			{
				if (index >= 1 && Input[index - 1] == '[')//方向键
				{
					deleteArr(Input, length--, index - 1);
					index--;
					switch (ch)
					{
					case 'A': //方向键上
						if (historyP>0)
						{
							historyP--;
							strcpy(Input, history[historyP]);
							clearLine(length + 1);
							index = length = strlen(Input);
						}
						break;
					case 'B'://方向键下
						clearLine(length + 1);
						if (historyP<historysize)
						{
							historyP += 1;
							strcpy(Input, history[historyP]);
							index = length = strlen(Input);
						}
						else
						{
							strcpy(Input, "");
							index = length = 0;
						}
						break;
					}
					ch = '\0';
					continue;
				}
				else
				{
					insertArr(Input, length, index, ch);
					index++;
					length++;
				}
			}
			else if (ch == 9 && index == length)
			{                                             //补全指令
				char* myinput = Input;
				int cou;
				int length = strlen(myinput);
				struct dirent * dirP;
				DIR * dirSS = opendir(PWD);
				char namestr[NAME_LENGTH];
				for (cou = 0; cou<length; cou++)
				{
					if (myinput[cou] == ' ' && (cou + 1<length))
						myinput = myinput + cou + 1;
				}
				length = strlen(myinput);

				while (dirP = readdir(dirSS))
				{
					strcpy(namestr, dirP->d_name);
					if (strlen(namestr) >= length)
					{
						namestr[length] = '\0';
						if (!strcmp(namestr, myinput))
						{
							strcpy(myinput, dirP->d_name);
							for (cou = length; cou>0; cou--)
								printf("\b \b");

							puts(myinput);
							return;
						}
					}
				}
				strcpy(namestr, myinput);
				strcat(namestr, "*");

				struct List* pList = (struct List*)newlist();

				searchtree(TREE, namestr, pList);
				struct ListNode * pNode = pList->head;
				if (pNode)
				{
					printf("\n");
					int i = 0;
					while (pNode)
					{
						printf("%s      ", pNode->content);
						i += 1;
						if (i == 5)
						{
							printf("\n");
							i = 0;
						}
						pNode = pNode->next;
					}
					printf("\n%s ", COMP);
					puts(myinput);
				}
				index = strlen(Input);
			}
			else if (ch == 127 && index>0)
			{
				deleteArr(Input, length, index - 1);
				length--;
				index--;
				continue;
			}
		} while (ch != '\n');

		puts("\n");
		Input[index] = '\0';

		if (strlen(Input) == 0)continue;
		strcpy(history[historysize++], Input);
		historyP = historysize;

		devideInput(Input);
	}
	exit(0);
}


void inputCommand(char buf[BUF_LENGTH], int argc, char *const argv[])//执行输入
{
	int pid = fork();
	if (pid != 0)
	{
		waitpid(pid, NULL, 0);
	}
	else
	{
		char shuru[] = "shurutempfile-XXXXXX";
		char shuchu[] = "shuchutempfile-XXXXXX";
		int fd_in = open(shuru, O_RDONLY);
		int fd_out = open(shuchu, O_CREAT | O_RDWR);

		write(fd_in, buf, BUF_LENGTH);
		if (fd_in > 0 && fd_out > 0)
		{
			dup2(fd_in, 0);
			dup2(fd_out, 1);
			RUN(argc, argv);
			close(fd_in);
			close(fd_out);
		}
		remove(shuru);
		remove(shuchu);
	}
}

int cd(char * path)//指令cd
{
	if (path == NULL)      //cd后面不加路径默认为主目录
	{
		path = "~";
	}
	if (*path == '~')        //若有～，转换成实际地址
	{
		char tempath[MAX_PATH];
		strcpy(tempath, HOME);
		strcat(tempath, &(path[1]));
		path = tempath;
	}
	if (chdir(path) == -1)//chdir():更改当前目录
	{
		printf("%s :", path);
		printerror(errno);
	}
	UPDATE();
	return 0;
}