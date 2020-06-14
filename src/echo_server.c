#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define BUF_SIZE 1024
void error_handling(char *message);
void reverse_message(char *reverse, char *message);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	char message[BUF_SIZE];
	char reverse[BUF_SIZE];
	char message_time[BUF_SIZE];
	int str_len, i;

	time_t timer;
	struct tm *t;

	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr;
	socklen_t clnt_adr_sz;

	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock==-1)
		error_handling("socket() error");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");

	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");

	clnt_adr_sz=sizeof(clnt_adr);

	for(i=0; i<5; i++)
	{
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);

		if(clnt_sock==-1)
			error_handling("accept() error");
		else
			printf("Connected client %d \n", i+1);

		while((str_len=read(clnt_sock, message, BUF_SIZE))!=0)
		{
			// 현재 시간
			timer = time(NULL);
			t = localtime(&timer);

			// Character 변수 초기화
			message[str_len]=0;
			reverse[str_len]=0;
			message_time[strlen(message_time)]=0;

			reverse_message(reverse, message);
			printf("Reverse Message: %s", reverse);
			// reverse 된 문자열과 시간을 같이 보냄
			sprintf(message_time, " %sDate: %d.%d.%d %d시 %d분 %d초\n", reverse, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
			write(clnt_sock, message_time, strlen(message_time));
		}

		close(clnt_sock);
	}

	close(serv_sock);
	return 0;
}

void reverse_message(char *reverse, char *message)
{
	/*
	* 문자열 reverse 함수
	* call by reference
	*/
	int reverse_idx = 0;

	for (int i = strlen(message)-2; i >= 0; i--)
	{
		reverse[reverse_idx] = message[i];
		reverse_idx++;
	}
	reverse[reverse_idx] = '\n';
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
