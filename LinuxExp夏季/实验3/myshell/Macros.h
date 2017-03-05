#ifndef MACROS_H
#define MACROS_H


#define NAME_LENGTH     64                          // �û������ļ����ȵ���󳤶�
#define PROMPT_LENGTH   64                          // ������ʾ����󳤶�
#define VAR_LENGTH      256                         // ��������󳤶�
#define PARAMETER 128                               //��������
#define PROCESS 128                                 //��������
#define COMMAND     2048                            // �ⲿ�����������
#define VAR_MAX         1024                        // ���ر���������
#define MAXLINE    80
#define HISTORY_MAX     128                         // ��󱣴���ʷ��¼����
#define COMMAND_IN     128                         // �ڽ������������
#define BUF_LENGTH      1024                        // ������󳤶�
#define INPUT_MAX       256                         // һ��������������󳤶�
#define MAX_PATH     256                        // ·����󳤶�
#define NODE_SIZE 40
typedef char WORD[NAME_LENGTH];

char history[HISTORY_MAX][INPUT_MAX];            //������ʷ
int  historysize, historyP;                      //�й���ʷ����Ϣ

#endif
