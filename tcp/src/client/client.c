#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>


#define MAXLINE 4096
#define CLIENT_NAME "#name"

int srv_fd;
int clt_fd;
int ret;
struct sockaddr_in srv_addr;
struct sockaddr_in clt_addr;
socklen_t clt_addr_len;

void init_connect(char** argv);
void str_cli(FILE *fp, int sock_fd);
void data_deal(char *buf);



/*************************************************************************************************/
/**********************************网络初始化socket************************************************/
/*************************************************************************************************/
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
    srv_addr.sin_port = htons(6660);
	
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


/*************************************************************************************************/
/**********************************读写处理************************************************/
/*************************************************************************************************/
void str_cli(FILE *fp, int sock_fd)
{
	int			max_fd, stdineof;
	fd_set		rfd;
	char		buf[MAXLINE];
	int		n;
	char	tag[] = "[owen]";
 
	stdineof = 0;
	FD_ZERO(&rfd);

	FD_SET(sock_fd, &rfd);
	max_fd = ((fileno(fp) > sock_fd)?fileno(fp): sock_fd) + 1;
		
	
	while(1)
	{
		if (stdineof == 0)
			FD_SET(fileno(fp), &rfd);
		switch(select(max_fd, &rfd, NULL, NULL, NULL))
		{
			case 0:
				perror("select timeout!!!\n");
				break;
			case -1:
				perror("select error\n");
				break;
			default:
				if (FD_ISSET(sock_fd, &rfd)) 
				{	
					memset(buf, 0, sizeof(buf));
					if ( (n = read(sock_fd, buf, MAXLINE)) == 0) /* socket is readable */
					{
						if (stdineof == 1)
							return;		/* normal termination */
						else
							printf("str_cli: server terminated prematurely");
						close(sock_fd);
						printf("close server, please reconnect!!!\n");
					}
					write(fileno(stdout), buf, n);
				}
		 
				if (FD_ISSET(fileno(fp), &rfd))  /* input is readable */
				{  
					write(fileno(stdout), tag, sizeof(tag));
					if ( (n = read(fileno(fp), buf, MAXLINE)) == 0) 
					{
						stdineof = 1;
						//shutdown(sock_fd, SHUT_WR);	/* send FIN */
						FD_CLR(fileno(fp), &rfd);
						continue;
					}
		 
					write(sock_fd, buf, n);
				}
				break;
		}
		
	}
}


void data_deal(char *buf)
{

}

int main(int argc, char** argv)
{
		
	init_connect(argv);
	char sendbuf[MAXLINE];

	printf("send:");
	str_cli(stdin, srv_fd);

}