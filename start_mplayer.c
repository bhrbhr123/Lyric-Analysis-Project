#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
//����mplayer������ 
//����song_path Ϊ������·��
void mplayer_play(char * song_path)
{
	pid_t pid;
	pid=fork();
	if(pid<0)
	{
		perror("fork");
	}
	else if(pid==0)
	{

		close(1);
		close(2);
		execlp("mplayer","mplayer","-slave",song_path,NULL);
		exit(0);
	}
	else
		;
}

// ,"-quiet"