#include "myshell.h"



void insertArr(char* array,int len,int pos,char ch)//�������в���
{
	int i;
	for (i = len; i>pos; i--)
	{
		array[i] = array[i - 1];
	}
	array[i] = ch;
}


void deleteArr(char* array,int len,int pos)//��������ɾ��
{
	if (pos>len) 
		return;

	int i;
	for (i = pos; i<len - 1; i++)
	{
		array[i] = array[i + 1];
	}
	array[i] = '\0';
}

struct ProArray* newarray()//�����µĽ�������
{
	struct ProArray* newPA = (struct ProArray*)malloc(sizeof(struct ProArray));
	newPA->size = 0;
	newPA->head = -1;
	newPA->tail = -1;
	memset(newPA->Data, 0, PROCESS*sizeof(struct Process));
	return newPA;
}

void insertPA(struct ProArray * PA, struct Process item)//�������
{		
	PA->size++;
	PA->tail++;
	PA->Data[PA->tail] = item;
}

struct Process deletePA(struct ProArray * PA)//ɾ������
{
	PA->size--;
	struct Process data = PA->Data[PA->head];
	PA->head--;
	return data;
}

struct List* newlist()//�����µ�����
{
	struct List* list;
	list = (struct List*)malloc(sizeof(struct List));

	list->number = 0;
	list->head = list->tail = NULL;

	return list;
}



void insert(struct List* mylist,char *path,WORD word)//�������ڵ�
{
	struct ListNode * newnode = (struct ListNode *)malloc(sizeof(Node));

	newnode->next = NULL;

	strcpy(newnode->content, word);
	strcpy(newnode->path, path);

	if (mylist->head == NULL)
	{                    
		mylist->head = mylist->tail = newnode;
		mylist->number = 1;
	}
	else   
	{                                     
		mylist->tail->next = newnode;
		mylist->tail = newnode;
		mylist->number++;
	}
}

struct treenode * newnode()//�����µ��������ڵ�
{
	struct treenode * node = (struct treenode*)malloc(sizeof(struct treenode));
	node->number = 0;
	for (int i = 0; i<NODE_SIZE; i++)
	{
		node->next[i] = NULL;
	}
	node->letter = 0;
	node->isword = 0;
	return node;
}

void searchtree(struct treenode * Tree, char *Word, struct List* mylist)  //�����������Ƿ����Word
{
	int num = 0;
	int cou,index;
	if (Tree == NULL) 
		return;

	if (Word[num] >= 'a' && Word[num] <= 'z')
	{
		searchtree(Tree->next[Word[num] - 'a'], Word + 1, mylist);
	}
	else if (Word[num] == '*')
	{                  
		for (cou = 0; cou<NODE_SIZE; cou++)
		{
			searchtree(Tree->next[cou],Word,mylist);
			searchtree(Tree->next[cou],Word + 1,mylist);
		}
	}
	else if (Word[num] == '\0')
	{                     
		if (Tree->isword)
		{
			for (cou = 0; cou<Tree->number; cou++)
			{
				insert(mylist, Tree->path, Tree->word[cou]);
			}
		}
		return;
	}	
	else if (Word[num] == '?')            
	{                                   
		for (cou = 0; cou<NODE_SIZE; cou++)
		{
			searchtree(Tree->next[cou],Word + 1, mylist);
		}
	}
	
}



int isexist(char* word)             //���word�Ƿ�������
{
	struct List * mylist = (struct List*)newlist();
	searchtree(TREE,word, mylist);
	if (mylist->number == 0)
	{                 
		return 0;
	}

	return 1;
}

void treeinsert(WORD word, char* path,int script)//�����в���ڵ�
{
	struct treenode *tmpnode = TREE;
	int i, j, len;
	char mychar;
	len = strlen(word);
	for (i = 0; i<len; i++)
	{
		if (isalpha(word[i]))
		{
			mychar = word[i] | 0x20;        
			j = mychar - 'a';
		}
		else if (word[i] == '_')
		{
			j = 38;
		}
		else if (word[i] == '-')
		{
			j = 36;
		}		
		else if (word[i] == '.')
		{
			j = 37;
		}
		else if (isdigit(word[i]))
		{
			j = word[i] - '0' + 26;
		}
		else
			return;

		if (tmpnode->next[j] == NULL)
		{
			tmpnode->next[j] = newnode();
			tmpnode->next[j]->letter = mychar;
		}
		else if ((i == len - 1) && (tmpnode->next[j]->number < 4))
		{
			tmpnode->next[j]->isword = 1;
			strcpy(tmpnode->next[j]->word[tmpnode->next[j]->number], word);  
			strcpy(tmpnode->next[j]->path, path);
			tmpnode->next[j]->number++;
		}

		tmpnode = tmpnode->next[j];
	}
}

