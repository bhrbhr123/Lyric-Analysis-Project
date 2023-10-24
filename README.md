# Lyric-Analysis-Project
Linux terminal displays simple scrolling of lyrics, using C language encoding.

---

1. **Extract lrc file data, process and store** (提取lrc文件数据，处理储存)

   1.1 Lyrics lrc data extraction (歌词lrc 数据提取)

   1.2 The original data is divided according to rows and the address is stored (原始数据按照行分割，地址存储)

   1.3 Processing of the first 5 lines of lyrics (歌词前5行处理)

   1.4 After lyrics processing, insert them into the linked list sequentially (歌词处理后 顺序插入链表)

   1.5 Free up application space (释放申请空间)

---

2. **Establish a storage mapping area for process timing communication (shared memory)** (建立存储映射区，用于进程计时通信（共享内存）)

---

2.  **Set an unnamed semaphore for mutual exclusion between processes in the same group (anonymous storage mapping)** (设置无名信号量，用于同组进程间互斥（匿名存储映射）)

   3.1 Initialization of unnamed semaphore (无名信号量的初始化)

---

4.  **Create a storage mapping area to share the PID of the child process to facilitate the timing process to send detection signals** (创建存储映射区，用于共享子进程PID，方便计时进程发送检测信号)

---

5.  **Play .mp3 files** (播放.mp3文件)（exec）

---

6.  **Create 2 child processes** (创建2个子进程)

   6.1 **Subprocess 1 executes the timing display task (loop)** (子进程1执行计时显示任务 （循环）)

- display time (显示时间)

```shell
1）Read shared memory area data (读共享内存区数据)
2）Unnamed semaphore P operation (无名信号量 P操作)
3）Set cursor position (设置光标位置)
4）Set font color (设置字体颜色)
5）Display time format (显示时间格式)
6）Unnamed semaphore V operation (无名信号量 V操作)
```
- Delay 1 second (延时1秒)

- Update time (更新时间)

```shell
1）Shared memory area locked (共享内存区 上锁)
2）Shared memory area writing data (共享内存区 写数据)
3）Shared memory area unlocked (共享内存区 解锁)
```
- Send signal SIGSTKFLT to child process 2(给子进程2发送信号 SIGSTKFLT)

  

  6.2 **Subprocess 2 displays text lyrics** (子进程2显示正文歌词)

- Transmit its own process ID to the process that sent the heartbeat signal (storage mapping area) (传输自身进程ID 给发送检测信号进程（存储映射区）)

- Set custom signal handling SIGUSE1 (设置自定义信号处理 SIGUSE1)

```shell
1）Set signal handling flags (设置信号处理标志)
2）Set signal set (设置信号集)
3）Set signal processing function --> Set output flag check (设置信号处理函数 --> 置位输出标志check)
4）Signal handling reset (信号处理重置)
```
- Receive output signal and dynamically display lyrics (loop) (接收输出信号，动态显示歌词（循环）)

  6.3 **main process** (主进程)

```shell
Wait for child process (loop) (等待子进程（循环）)
```

---

