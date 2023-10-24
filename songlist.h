#ifndef __SONGLIST_H__
#define __SONGLIST_H__

#include <semaphore.h>
#include <signal.h>


// lrc文件存储位置
#define LRC_POSITION "简单爱.lrc"
// lrc文件最大行数
#define LRC_ROW_MAX 35



// 歌词节点结构
typedef struct song
{
    // 数据域
    char *lyric;     // 歌词
    int min;         // 分钟
    int sec;         // 秒数

    // 指针域
    struct song *prorad;        // 前向指针
    struct song *next;          // 后向指针
}SONG, *SP;

// 输出标志位
extern int check;



extern char *file_withdraw(void);                               // 歌词lrc 数据提取
extern char **segmentation(char *fwd);                          // 原始数据按照行分割
extern void BCLNK_list_add(SP *head, SP node_tmp);              // 双向循环链表添加
extern int my_strlen(char *str);                                // 计算字符串长度
extern void BCLNK_list_print(SP head);                          // 链表的遍历
extern char **handle_four(char **row_tok);                      // 歌词前4行处理
extern int handle_end(SONG **head, char **row_tok);             // 歌词链表处理
extern void song_handle(char ***frt_four, SONG **head);         // 歌词处理封装
extern void print_front(char **frt_four, sem_t *sem);           // 歌词前4行输出
extern int *get_reflection(void);                               // 建立映射函数
extern void time_delay(int sec);                                // 延时函数
extern void time_change(int *sec);                              // 修改时间函数
extern void display_time(int *sec, sem_t *sem);                 // 显示时间函数
extern void my_waitpid(void);                                   // 等待进程退出
extern void sig_fun(int signo, siginfo_t *info, void *ptr);     // 信号执行函数
extern void my_signal_handle(void);                             // 信号处理设置
extern void print_lyric(SONG **head, int *sec, sem_t *sem, char **frt_four);     // 正文歌词显示
extern void initial_print(SP head, sem_t *sem, int flag);       // 歌词初始化显示
extern void time_task(int *sec, sem_t *sem, pid_t *cmpid);      // 进程计时显示任务
// 进程歌词显示
extern void lyric_task(pid_t *cmpid, char **frt_four, sem_t *sem, SONG **head, int *sec);
// 进程创建
extern void progress_task(int *sec, pid_t *cmpid, char **frt_four, sem_t *sem, SP *head);

#endif