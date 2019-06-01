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
#include <pthread.h>

#define MAXLINE 4096
#define CLIENT_NAME "#name"



struct sockaddr_in srv_addr;


int init_connect(char* argv);
void str_cli(FILE *fp, int sock_fd);
void data_deal(char *buf);


/*************************************************************************************************/
/**********************************网络初始化socket************************************************/
/*************************************************************************************************/
int init_connect(char* argv)
{
	int srv_fd;
	
	struct sockaddr_in clt_addr;
	socklen_t clt_addr_len;
	
	clt_addr_len = (socklen_t)sizeof(clt_addr);
	
	//create socket
	srv_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == srv_fd)
	{
		printf("socket service faild: %s(errno: %d)\r\n",strerror(errno),errno);
		exit(0);
	}
	printf("socket create success and fd:%d!!!\r\n", srv_fd);
	
	//connected
	if(-1 == connect(srv_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)))
	{
		printf("socket connect faild: %s(errno: %d)\r\n",strerror(errno),errno);
		exit(0);		
	}

	printf("server[%s]Port[%d] connected!!!\r\n", inet_ntoa(srv_addr.sin_addr), srv_addr.sin_port);
	
	return srv_fd;
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

void *client_thread(void *arg)
{
	int srv_fd;
	char *ip_str = (char *)arg;
	
	
	srv_fd = init_connect(ip_str);
	char sendbuf[MAXLINE];

	printf("send:");
	str_cli(stdin, srv_fd);

	close(srv_fd);

	return NULL;

}

void client_thread_config(char *ip_str)
{
	pthread_t thread_id;
	int ret;

	ret = pthread_create(&thread_id, NULL, client_thread, (void *)&ip_str);
	if(0 != ret){
		printf("client pthrad_create faild: %s(errno: %d)\r\n",strerror(errno),errno);
		exit(0);
	}
	printf("thread create ok-id:%d\r\n", (int)thread_id);
	//pthread_join(thread_id, NULL);
}

int main(int argc, char** argv)
{
	/**
			Usage: ./client + ip + client num
	
	**/	
	int srv_fd;
	unsigned int i;
	pid_t pid;
	int thread_num;
	
	if(argc < 1)
	{
		printf("=======================Usage=========================\r\n");
		printf("========./main + ip or + thread client number========\r\n");
		printf("=====================================================\r\n");
		exit(0);
	}
	else if (argc == 3)
	{
		thread_num = atoi(argv[2]);
	}
		
	
	char *ip_str = argv[1];
		
	memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(6666);	
		
	if( inet_pton(AF_INET, argv[1], &srv_addr.sin_addr) <= 0){
		printf("inet_pton faild: %s(errno: %d)\r\n",strerror(errno),errno);
		exit(0);
    }
	
	if(thread_num > 0 && thread_num < 100)
	{
		for(i = 0; i < thread_num; i++)
		{
			//创建线程
			client_thread_config(ip_str);
		}
	}
	
	srv_fd = init_connect(ip_str);
	char sendbuf[MAXLINE];

	printf("send:");
	str_cli(stdin, srv_fd);
	
	close(srv_fd);

}