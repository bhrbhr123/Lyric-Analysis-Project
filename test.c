// 歌词解析项目
#include <stdio.h>
#include "songlist.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "console.h"
#include "start_mplayer.h"
             
#define SONG_PATH "song/简单爱.mp3"

int main(int argc, char **argv)
{
    /***** 变量定义 *****/
    char **frt_four = NULL;         // 存储歌词 前4行 (二层申请)
    SP head = NULL;                 // 定义双向循环链表的头指针
    int *sec = NULL;                // 映射区地址
    sem_t *sem;                     // 定义信号量
    static int fd = 0;              // 文件描述符
    pid_t *cmpid = NULL;            // 通讯用存储映射区

    // 建立映射区
    sec = get_reflection();

    // 设置无名信号量
    sem = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // 无名信号量的初始化  进程间互斥
    sem_init(sem, 1, 1);

    // 存储映射准备工作  打开文件描述符
    fd = open("cm_pid", O_CREAT | O_RDWR | O_TRUNC, 0666);      
    if(fd == -1)
    {
        perror("fd_open");
        return 0;
    }

    // 拓展文件大小
    truncate("cm_pid", sizeof(pid_t));          

    // 建立存储映射
    cmpid = (pid_t *)mmap(NULL, sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // 歌词处理
    song_handle(&frt_four, &head);

    // 播放.mp3文件（exec）
    mplayer_play(SONG_PATH);

    // 歌曲内容显示
    progress_task(sec, cmpid, frt_four, sem, &head);

    return 0;
}