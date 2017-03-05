#ifndef MACROS_H
#define MACROS_H


#define NAME_LENGTH     64                          // 用户名，文件名等的最大长度
#define PROMPT_LENGTH   64                          // 命令提示符最大长度
#define VAR_LENGTH      256                         // 变量的最大长度
#define PARAMETER 128                               //参数上限
#define PROCESS 128                                 //进程上限
#define COMMAND     2048                            // 外部命令最大条数
#define VAR_MAX         1024                        // 本地变量最大个数
#define MAXLINE    80
#define HISTORY_MAX     128                         // 最大保存历史记录条数
#define COMMAND_IN     128                         // 内建命令最大条数
#define BUF_LENGTH      1024                        // 缓存最大长度
#define INPUT_MAX       256                         // 一次命令行输入最大长度
#define MAX_PATH     256                        // 路径最大长度
#define NODE_SIZE 40
typedef char WORD[NAME_LENGTH];

char history[HISTORY_MAX][INPUT_MAX];            //输入历史
int  historysize, historyP;                      //有关历史的信息

#endif
