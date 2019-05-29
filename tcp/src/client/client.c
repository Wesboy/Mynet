#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAXLINE 4096

int srv_fd;
int clt_fd;
int ret;
struct sockaddr_in srv_addr;
struct sockaddr_in clt_addr;
socklen_t clt_addr_len;

void init_connect(char** argv);

void init_connect(char** argv)
{
	clt_addr_len = (socklen_t)sizeof(clt_addr);
	
	//create socket
	srv_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == srv_fd)
	{
		printf("socket service faild: %s(errno: %d)\r\n",strerror(errno),errno);
		exit(0);
	}
	printf("socket create success and fd:%d!!!\r\n", srv_fd);
	
	memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(6664);
	
	if( inet_pton(AF_INET, argv[1], &srv_addr.sin_addr) <= 0){
		printf("inet_pton error for %s\n",argv[1]);
		exit(0);
    }
	
	//connected
	if(-1 == connect(srv_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)))
	{
		printf("socket connect faild: %s(errno: %d)\r\n",strerror(errno),errno);
		exit(0);		
	}

	printf("server[%s]Port[%d] connected!!!\r\n", inet_ntoa(srv_addr.sin_addr), srv_addr.sin_port);
}

int main(int argc, char** argv)
{
		
	init_connect(argv);
	char sendbuf[MAXLINE];
	
	while(1)
	{
		printf("send:");
		fgets(sendbuf, MAXLINE-1, stdin);
		if( -1 == send(srv_fd, sendbuf, strlen(sendbuf), 0))
		{
			printf("send msg error\r\n");
			exit(0);
		}
		
	}
}