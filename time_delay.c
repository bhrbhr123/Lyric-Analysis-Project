#include <stdio.h>
#include <time.h>
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
int main(int argc, char *argv[])
{
	while(1)
	{
		printf("hello world\n");
		time_delay(3);
	}
	return 0;
}




