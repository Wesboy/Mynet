#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_CLIENT 10
#define MAX_BUFF_SIZE 4096

void client_thread_config(int clt_fd);

void *client_thread(void *arg)
{
	int client_fd = *((int *)arg);
	int count;
	char recvbuf[MAX_BUFF_SIZE];
	
	while(1)
	{
		count = recv(client_fd, recvbuf, MAX_BUFF_SIZE, 0);
		recvbuf[count] = '\0';
		printf("[client-%3d]recv msg===>>%s\n", client_fd, recvbuf);
	}
	close(client_fd);
	
	return NULL;
	
}

void client_thread_config(int clt_fd)
{
	pthread_t thread_id;
	int ret;
	
	ret = pthread_create(&thread_id, NULL, client_thread, (void *)&clt_fd);
	if(0 != ret){
		printf("client pthrad_create faild: %s(errno: %d)\r\n",strerror(errno),errno);
		exit(0);
	}
	printf("thread create ok-id:%d\r\n", (int)thread_id);
	//pthread_join(thread_id, NULL);
}

int main(int argc, char *argv[])
{
	//1 bind
	int srv_fd;
	int clt_fd = 0;
	int ret;
	struct sockaddr_in srv_addr;
	struct sockaddr_in clt_addr;
	socklen_t clt_addr_len;
	
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
    srv_addr.sin_port = htons(6664);
	
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
	
	while(1)
	{
		//3 accept
		if( (clt_fd = accept(srv_fd, (struct sockaddr*)NULL, NULL)) == -1){
			printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
			continue;
		}
		printf("===================================\n");
		printf("client[%s] connected!!!\n", inet_ntoa(clt_addr.sin_addr));
		printf("===================================\n");
		
		//create thread
		client_thread_config(clt_fd);
	}
	
	close(srv_fd);
	
	return 0;
}