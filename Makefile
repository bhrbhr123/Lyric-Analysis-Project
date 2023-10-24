CC=gcc			# 指定编译器
EXEC=main		# 指定可执行文件名
OBJ=console.o start_mplayer.o test.o songlist.o 	# 生成的二进制文件名
OPTIONS=-Wall 	# 显示所有警告
LIB=-lpthread	# 包含线程使用库

# 二进制文件 链接生成可执行文件
$(EXEC):$(OBJ)
	$(CC) $^ -o $@ $(OPTIONS) $(LIB) 

# 源文件 生成 二进制文件
%.o:%.c
	$(CC) -c $< -o $@ $(OPTIONS) $(LIB)

# 清除中间文件 可执行文件
clean:
	rm -fr *.o $(EXEC)