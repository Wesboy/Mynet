
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

#define MAX_CLIENT 10
#define MAX_BUFF_SIZE 4096
#define CLIENT_NAME "#name"


struct client{
	int clt_fd;
	char name[20];
	char online;
};

static struct client clt[MAX_CLIENT];


int main(int argc, char *argv[])
{
	//1 bind
	int srv_fd;
	int new_clt_fd = 0;
	int ret;
	int i;
	struct sockaddr_in srv_addr;
	struct sockaddr_in clt_addr;
	socklen_t clt_addr_len;
	clt_addr_len = sizeof(struct sockaddr_in);
	
	fd_set rfd;
	int max_fd;
	char recvbuf[MAX_BUFF_SIZE];
	struct timeval tv;
	
	//1 create socket
	srv_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == srv_fd)
	{
		printf("socket service faild: %s(errno: %d)\r\n",strerror(errno),errno);
		exit(0);
	}
	
	memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(6660);
	
	ret = bind(srv_fd, (struct sockaddr *)&srv_addr, (socklen_t)sizeof(srv_addr));
	if(ret == -1)
	{
		printf("bind service faild: %s(errno: %d)\r\n",strerror(errno),errno);
		exit(0);
	}
	//2 listen
	ret = listen(srv_fd, MAX_CLIENT);
	if(ret == -1)
	{
		printf("listen service faild\r\n");
		exit(0);
	}
	
	printf("listening wait!!!!\r\n");
	
	max_fd = srv_fd;
	i = 0;
	tv.tv_sec = 50;
	tv.tv_usec = 0;

	while(1)
	{
		
		FD_ZERO(&rfd);		
		FD_SET(srv_fd, &rfd);

		for(i = 0; i < MAX_CLIENT; i++)
		{
			if(clt[i].clt_fd != 0)
				FD_SET(clt[i].clt_fd , &rfd);
		}
		
		
		ret = select(max_fd+1, &rfd, NULL, NULL, NULL);
		
		
		if(0 == ret)
		{
			printf("select timeout\r\n");
			continue;
		}
		else if(-1 == ret)
		{
			printf("select error: %s(errno: %d)",strerror(errno),errno);
		}
		else
		{
			for(i = 0; i < MAX_CLIENT; i++)
			{
				if(FD_ISSET(clt[i].clt_fd, &rfd))
				{
					ret = recv(clt[i].clt_fd, recvbuf, MAX_BUFF_SIZE, 0);
					
					if(ret <= 0){
						printf("client[%d] close!!!\r\n", i);
						close(clt[i].clt_fd);
						FD_CLR(clt[i].clt_fd, &rfd);
						clt[i].clt_fd = 0;
					}
					else
					{
						if(ret < MAX_BUFF_SIZE)
						{
							recvbuf[ret] = '\0';
							printf("[client-%3d]recv msg===>>%s\n", clt[i].clt_fd, recvbuf);
						}
						else
						{
							printf("over buff!!!\r\n");
						}
					}
				}
			}
			if(FD_ISSET(srv_fd, &rfd))
			{
				// accept
				if((new_clt_fd = accept(srv_fd, (struct sockaddr*)&clt_addr, &clt_addr_len)) == -1){
					printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
				}
				printf("connnected!!\r\n");
				
				for(i = 0; i<MAX_CLIENT;i++)
				{
					if(clt[i].clt_fd == 0)
					{
						if(i < MAX_CLIENT)
						{
							clt[i].clt_fd = new_clt_fd;
							printf("===================================\n");
							printf("client[%d][ip:%s][Port:%d] connected!!!\n", clt[i].clt_fd, inet_ntoa(clt_addr.sin_addr), ntohs(clt_addr.sin_port));
							printf("===================================\n");
							
							send(clt[i].clt_fd, );
							
							if(clt[i].clt_fd > max_fd)
							{
								max_fd = clt[i].clt_fd;
							}
							break;
						}
						else
						{
							printf("over max client!!!\r\n");
							close(new_clt_fd);
							break;
						}
					}
				}
			}
		}	
	}
	close(srv_fd);
	
	return 0;
}