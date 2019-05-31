
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
	char bAuthentication;
};



int max_fd;
static struct client clt[MAX_CLIENT];



static void client_data();
static void wait_accept(int srv_fd);
static int create_socket();
static void client_authentication(struct client *clt, char *buf, int len);

/*************************************************************************************************/
/**********************************网络初始化socket************************************************/
/*************************************************************************************************/
static int create_socket()
{
	int ret;
	int srv_fd;
	struct sockaddr_in srv_addr;
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
    srv_addr.sin_port = htons(6663);
	
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
	
	return srv_fd;
}

static void wait_accept(int srv_fd)
{
	int i;
	int new_clt_fd = 0;
	struct sockaddr_in clt_addr;
	socklen_t clt_addr_len;
	
	
	char send_str[] = "Please input your name:";
	
	clt_addr_len = sizeof(struct sockaddr_in);
	
	// accept
	if((new_clt_fd = accept(srv_fd, (struct sockaddr*)&clt_addr, &clt_addr_len)) == -1){
		printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
	}
	
	for(i = 0; i<MAX_CLIENT;i++)
	{
		if(clt[i].clt_fd == 0)
		{
			
			clt[i].clt_fd = new_clt_fd;
			clt[i].bAuthentication = 0; //未认证			
			clt[i].online = 1;
			printf("===================================\n");
			printf("client[%d][ip:%s][Port:%d] connected!!!\n", clt[i].clt_fd, inet_ntoa(clt_addr.sin_addr), ntohs(clt_addr.sin_port));
			printf("===================================\n");

			send(clt[i].clt_fd, send_str, sizeof(send_str), 0);

			
			if(clt[i].clt_fd > max_fd)
			{
				max_fd = clt[i].clt_fd;
			}
			break;
		}
	}
}

static void client_authentication(struct client *clt, char *buf, int len)
{
	int i = 0;
	char warn_info[] = "Please input name less 10 char!!!\n";
	
	if(len > 10 && len <= 0)
	{
		send(clt->clt_fd, warn_info, sizeof(warn_info), 0);
		return;
	}
	else
	{
		//去掉回车符
		i = 0;
		clt->name[0] = '[';
		while(buf[i] != '\n')
		{
			clt->name[i+1] = buf[i];
			i++;
		}
		clt->name[i+1] = ']';i++;
		clt->name[i+1] = '\0';
		clt->bAuthentication = 1;
	}
}


static void client_data(struct client *clt)
{
	int i;
	int ret;
	char recvbuf[MAX_BUFF_SIZE];
	
	ret = recv(clt->clt_fd, recvbuf, sizeof(recvbuf), 0);
	
	if(ret <= 0){
		printf("client[%d] close!!!\r\n", clt->clt_fd);
		close(clt->clt_fd);
		clt->online = 1;
		clt->clt_fd = 0;
	}
	else
	{
		if(ret < MAX_BUFF_SIZE)
		{
			if(!clt->bAuthentication)
			{
				client_authentication(clt, recvbuf, ret);
			}
			else{
				//printf("[%s]%s", clt->name, recvbuf);
				write(fileno(stdout), clt->name, strlen(clt->name));
				write(fileno(stdout), recvbuf, ret);
			}
		}
		else
		{
			printf("over buff!!!\r\n");
		}
	}	
}

int main(int argc, char *argv[])
{

	int srv_fd;
	fd_set rfd;
	int i;
	
	
	memset(clt, 0, sizeof(clt));
	
	srv_fd = create_socket();
	max_fd = srv_fd;
	
	while(1)
	{
		
		FD_ZERO(&rfd);		
		FD_SET(srv_fd, &rfd);

		for(i = 0; i < MAX_CLIENT; i++)
		{
			if(clt[i].clt_fd != 0){
				FD_SET(clt[i].clt_fd , &rfd);
			}
			else{
				FD_CLR(clt[i].clt_fd , &rfd);
			}
		}
		
		switch(select(max_fd+1, &rfd, NULL, NULL, NULL))
		{
			case 0:
				printf("select timeout\r\n");
				break;
			case -1:
				printf("select error: %s(errno: %d)",strerror(errno),errno);
				break;
			default:
			{
				for(i = 0; i < MAX_CLIENT; i++)
				{
					if(FD_ISSET(clt[i].clt_fd, &rfd))
					{
						client_data(&clt[i]);
					}
				}
				if(FD_ISSET(srv_fd, &rfd))
				{
					wait_accept(srv_fd);
					
				}
			}
		}	
	}
	close(srv_fd);
	
	return 0;
}