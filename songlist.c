#include "songlist.h"
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "console.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/wait.h>


int check = 0;                         // 输出标志位
static key_t key = 0;                  // 定义key值类型
static int shm_flag = 0;               // 共享内存标志符

char *file_withdraw(void)
{
    /* 变量定义 */
    struct stat *sta = NULL;            // 文件大小所需结构体
    int fd = 0;                         // 文件描述符
    int file_size = 0;                  // 文件大小备份
    char *fwd = NULL;                   // 文件原始数据存储空间

    // 申请sta所需空间
    sta = (struct stat *)calloc(1, sizeof(struct stat));
    if (sta == NULL)
    {
        perror("stat_calloc");
        goto ERROR;
    }

    // 检测文件大小
    stat(LRC_POSITION, sta);
    file_size = sta->st_size;

    // 通过文件大小 申请 接收区动态大小
    fwd = (char *)calloc(1, (sta->st_size + 20) * sizeof(char));
    if (fwd == NULL)
    {
        perror("fwd_calloc");
        goto ERROR;
    }

    // 释放 文件大小所需结构体sta
    if (sta == NULL)
    {
        perror("sta_free");
        goto ERROR;
    }
    free(sta);

    // 以只读方式打开结构体
    fd = open(LRC_POSITION, O_RDONLY);
    if (fd == -1)
    {
        perror("fd_open");
        goto ERROR;
    }

    // 读取数据 到存储空间
    read(fd, fwd, file_size);

    // 关闭文件描述符
    close(fd);


    return fwd;

// 错误跳转
ERROR:
    return (char *)EOF;
}

char **segmentation(char *fwd)
{
    /* 变量定义 */
    char **row_tok = NULL;              // 原始数据按照行分割 地址存储
    int i = 0;

    // 按行分割 分配空间
    row_tok = (char **)calloc(LRC_ROW_MAX, sizeof(char *));
    if (row_tok == NULL)
    {
        perror("row_tok_calloc");
        goto ERROR;
    }
    
    // 歌词按照行分割
    i = 0;
    row_tok[i] = fwd;
    while ((row_tok[i] = strtok(row_tok[i], "\n")) && ++i);

    return row_tok;

// 错误跳转
ERROR:
    return (char **)EOF;
}

int handle_end(SONG **head, char **row_tok)
{
    /* 变量定义 */
    SP tmp_node = NULL;             // 临时节点
    int i = 0;                      // 循环变量
    char *ptr = NULL;               // 地址临时保存变量



    // [03:34.64][02:34.71][01:05.83]我想就这样牵着你的手不放开
    // 创建临时节点空间
    tmp_node = (SP)calloc(1, sizeof(SONG));
    if (tmp_node == NULL)
    {
        perror("node_calloc");
        goto ERROR;
    }
    tmp_node->next = NULL;
    tmp_node->prorad = NULL;

    for (i = 4; row_tok[i] != NULL; i++)
    {
        ptr = strrchr(row_tok[i], ']');
        if (ptr != NULL)
        {
            // 创建歌词空间
            tmp_node->lyric = (char *)calloc(1, my_strlen(ptr) * sizeof(char));
            if (tmp_node->lyric == NULL)
            {
                perror("lyric_calloc");
                goto ERROR;
            }

            // 拷贝歌词数据
            strcpy(tmp_node->lyric, ptr + 1);

            ptr = row_tok[i];
            while (1)
            {
                if (ptr[0] != '[')
                {
                    break;
                }

                // 解包
                sscanf(ptr, "[%d:%d", &tmp_node->min, &tmp_node->sec);

                // 把节点 添加到链表中
                BCLNK_list_add(head, tmp_node);

                ptr += 10;
            }
        }
        else
        {
            break;
        }
    }




    return 0;

// 错误跳转
ERROR:
    return EOF;
}


// 双向循环链表添加
void BCLNK_list_add(SP *head, SP node_tmp)
{
    SP ptr_frt = NULL;
    SP ptr_bak = NULL;

    // 接收头指针
    SP head_p = *head;

    // 创建一个节点
    SP node = (SP)calloc(1, sizeof(SONG));
    if (node == NULL)
    {
        perror("calloc");
        return;
    }

    // 接收数据
    *node = *node_tmp;

    // 判断链表存在
    if (head_p != NULL)
    {
        // 链表存在 顺序插入
        if ((node->min > head_p->min) || ((node->min == head_p->min) && (node->sec > head_p->sec)))
        {
            // 大于头结点
            do
            {
                head_p = head_p->next;
            } while(((node->min > head_p->min) || ((node->min == head_p->min) && (node->sec > head_p->sec))) && head_p != *head);

            // 找到插入点  保存插入两端结点的地址
            ptr_frt = head_p->prorad;
            ptr_bak = head_p;

            // 插入结点
            ptr_frt->next = node;
            node->next = ptr_bak;
            ptr_bak->prorad = node;
            node->prorad = ptr_frt;

        }
        else    // 小于头结点
        {
            // 保存插入两端结点的地址
            ptr_frt = head_p->prorad;
            ptr_bak = head_p;

            // 插入结点
            ptr_frt->next = node;
            node->next = ptr_bak;
            ptr_bak->prorad = node;
            node->prorad = ptr_frt;

            // 更新头结点位置
            *head = node;
        }

    }
    else
    {
        // 链表不存在
        node->prorad = node;
        node->next = node;
        *head = node;
    }
    return;
}

int my_strlen(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        i++;
    }

    return i;
}

// 链表的遍历
void BCLNK_list_print(SP head)
{
    // 保存头部指针
    SP head_p = head;

    // 判断链表存在
    if (head == NULL)
    {
        printf("链表不存在\n");
        return;
    }

    // 链表数据遍历
    do
    {
        printf("min:%d  sec:%d  song:%s\n", head->min, head->sec, head->lyric);
        head = head->next;
    } while (head != head_p);
    
    return;
}

char **handle_four(char **row_tok)
{
    /* 变量定义 */
    int i = 0;                          // 循环变量
    char **frt_four = NULL;             // 前4行存储地址
    char str[50] = "";                  // 字符串处理临时变量

    // 为存储地址 申请空间
    frt_four = (char **)calloc(4, sizeof(char *));
    if (frt_four == NULL)
    {
        perror("frt_four_calloc");
        goto ERROR;
    }

    // 歌词前四行 变换转移
    for (i = 0; i < 4; i++)
    {
        // 清空字符串
        memset(str, 0, sizeof(str) * sizeof(char));

        // 解包 出有用信息
        sscanf(row_tok[i], "%*4c%[^]]", str);

        // 申请空间
        frt_four[i] = (char *)calloc(1, 30 * sizeof(char));
        if (frt_four[i] == NULL)
        {
            perror("frt_four_calloc");
            goto ERROR;
        }

        // 拼装信息
        switch (i)
        {
            case 0:
                // 组织信息
                sprintf(frt_four[i], "歌名:\t%s", str);
                break;
            case 1:
                // 组织信息
                sprintf(frt_four[i], "歌手:\t%s", str);
                break;
            case 2:
                // 组织信息
                sprintf(frt_four[i], "专辑:\t%s", str);
                break;
            case 3:
                // 组织信息
                sprintf(frt_four[i], "制作:\t%s", str);
                break;
            default:
        }
    }

    return frt_four;

// 错误跳转
ERROR:
    return (char **)EOF;
}

void song_handle(char ***frt_four, SONG **head)
{
    /* 变量定义 */
    char *fwd = NULL;                   // 文件原始数据存储空间
    char **row_tok = NULL;              // 原始数据按照行分割 地址存储

    // 歌词lrc 数据提取
    fwd = file_withdraw();

    // 原始数据按照行分割 地址存储
    row_tok = segmentation(fwd);

    // 歌词前4行处理
    *frt_four = handle_four(row_tok);

    // 歌词处理后 顺序插入链表
    handle_end(head, row_tok);

    // 释放申请空间
    if (fwd != NULL && row_tok != NULL)
    {
        free(fwd);
        free(row_tok);
    }
    return;
}

void print_front(char **frt_four, sem_t *sem)
{
    int i = 0;

    

    // 隐藏光标
    cusor_hide();

    // P操作
    sem_wait(sem);

    // 清屏
    clear_screen();

    // 光标设置颜色 蓝
    set_fg_color(COLOR_BLUE);

    for (i = 0; i < 4; i++)
    {
        // 光标跳转
        cusor_moveto(30, i + 1);

        // 输出歌词
        printf("%s\n", frt_four[i]);
    }

    // V操作
    sem_post(sem);

    return;
}


int *get_reflection(void)
{
    int *sec = NULL;

    // 获取系统唯一key值
    key = ftok("/", 666);
    if (key == -1)
    {
        perror("key_ftok");
        goto ERROR;
    }

    // 申请共享内存区
    shm_flag = shmget(key, sizeof(int), IPC_CREAT | 0666);

    // 建立映射关系
    sec = (int *)shmat(shm_flag, NULL, 0);

    return sec;

// 错误跳转
ERROR:
    return (int *)EOF;
}


void time_delay(int sec)
{
	int s_time,e_time;
	s_time=time(NULL);
	while(1)
	{
		e_time=time(NULL);
		if(e_time==s_time+sec)
			break;
	}
}

void time_change(int *sec)
{
    // 上锁
    shmctl(shm_flag, SHM_LOCK, NULL);

    // 秒数加1
    (*sec)++;

    // 解锁
    shmctl(shm_flag, SHM_UNLOCK, NULL);

    return;
}

void display_time(int *sec, sem_t *sem)
{
    // 时间处理
    int min = (*sec) / 60;
    int s = (*sec) % 60;

    // 信号量 P 操作
    sem_wait(sem);

    // 设置光标位置
    cusor_moveto(35, 5);

    // 设置字体颜色 青
    set_fg_color(COLOR_CYAN);

    // 显示时间
    printf("%02d:%02d\n", min, s);

    // 信号量 V 操作
    sem_post(sem);

    return;
}

void my_waitpid(void)
{
    pid_t pid = 0;

    while (1)
    {
        // 等待退出进程
        pid = waitpid(-1, NULL, WNOHANG);

        if (pid > 0)
        {
            printf("成功回收进程:%d\n", pid);
        }
        else if (0 == pid)
        {
            continue;
        }
        else if (-1 == pid)
        {
            break;
        }
    }
    return;
}

void my_signal_handle(void)
{
    // 定义信号集
    sigset_t sigset;

    // 定义信号处理结构体
    struct sigaction sa, osa;

    // 设置信号处理标志
    sa.sa_flags = SA_SIGINFO;

    // 设置信号集
    sigemptyset(&sigset);
    sa.sa_mask = sigset;

    // 设置信号处理函数
    sa.sa_sigaction = sig_fun;

    // 信号处理重置
    sigaction(SIGSTKFLT, &sa, &osa);
}

void sig_fun(int signo, siginfo_t *info, void *ptr)
{
    // 置位输出标志
    check = 1;
    return;
}

void print_lyric(SONG **head, int *sec, sem_t *sem, char **frt_four)
{
    // 时间处理
    int min = (*sec) / 60;
    int s = (*sec) % 60;
    int i = 0;
    SP head_p = *head;
    int pos = 0;

    // 判断时间匹配
    for (i = 0; i < 5; i++, head_p = head_p->next)
    {
        if((head_p->min == min) && (head_p->sec == s))
        {
            break;
        }
    }
    if (5 == i)
    {
        return;
    }

    // 时间匹配，重置头指针
    head_p = *head;

    if (i == 3)
    {
        *head = (*head)->next;
        print_front(frt_four, sem);
        display_time(sec, sem);
        initial_print(*head, sem, 1);
        return;
    }

    

    // 信号量P操作
    sem_wait(sem);

    // 找到时间匹配节点
    pos = i + 6;
    for (i += 1; i > 0; i--, head_p = head_p->next)
    {
        if (1 == i)
        {
            // 设置光标位置
            cusor_moveto(27, pos);

            // 设置字体颜色 红
            set_fg_color(COLOR_RED);

            // 输出
            printf("%s\n", head_p->lyric);

            break;
        }
    }

    // 信号量V操作
    sem_post(sem);

    return;
}

void initial_print(SP head, sem_t *sem, int flag)
{
    int i = 0;

    // 信号量P 操作
    sem_wait(sem);

    for (i = 0; i < 5; i++)
    {
        // 设置光标位置
        cusor_moveto(27, i + 6);

        if (1 == flag && 2 == i)
        {
            // 设置字体颜色 红
            set_fg_color(COLOR_RED);
        }
        else
        {
            // 设置字体颜色 白
            set_fg_color(COLOR_WHITE);
        }

        // 输出
        printf("%s\n", head->lyric);
        head = head->next;
    }

    // 信号量V 操作
    sem_post(sem);
    return;
}

void time_task(int *sec, sem_t *sem, pid_t *cmpid)
{
    sigval_t sv;

    while (1)
    {
        // 显示时间
        display_time(sec, sem);

        // 延时1秒
        time_delay(1);

        // 修改数据
        time_change(sec);

        // 发送信号
        sigqueue(*cmpid, SIGSTKFLT, sv);
    }
    return;
}

void lyric_task(pid_t *cmpid, char **frt_four, sem_t *sem, SONG **head, int *sec)
{
    // 传输自身进程ID
    *cmpid = getpid();
    
    // 设置自定义信号处理 SIGUSE1
    my_signal_handle();

    // 初始打印
    print_front(frt_four, sem);
    initial_print(*head, sem, 0);
    
    while (1)
    {
        if(check == 1)
        {
            check = 0;

            // 显示歌词
            print_lyric(head, sec, sem, frt_four);
        }
    }
    return;
}

void progress_task(int *sec, pid_t *cmpid, char **frt_four, sem_t *sem, SP *head)
{   
    int i = 0;                      // 循环变量
    pid_t pid = 0;                  // 定义进程号类型

    // 创建多进程 2
    for (i = 0; i < 2; i++)
    {
        pid = fork();
        if (0 == pid)
            break;
    }

    // 进程执行任务
    *sec = 0;
    if (0 == i)
    {
        // 显示正文歌词任务
        lyric_task(cmpid, frt_four, sem, head, sec);
    }
    else if (1 == i)
    {
        // 计时任务  
        time_task(sec, sem, cmpid);
    }
    else if (2 == i)
    {
        // 等待进程退出
        my_waitpid();
    }
    return;
}