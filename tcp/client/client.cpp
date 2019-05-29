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

int main(int argc, char** argv)
{
		
	int srv_fd;
	int clt_fd;
	int ret;
	struct sockaddr_in srv_addr;
	struct sockaddr_in clt_addr;
	socklen_t clt_addr_len;
	
	
	//1 create socket
	clt_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == clt_fd)
	{
		printf("socket service faild: %s(errno: %d)\r\n",strerror(errno),errno);
		exit(0);
	}
	
	memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(6665);
	
	if( inet_pton(AF_INET, argv[1], &srv_addr.sin_addr) <= 0){
		printf("inet_pton error for %s\n",argv[1]);
		exit(0);
    }
	
	//2 connected
	if(-1 == connect(clt_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)))
	{
		printf("socket connect faild: %s(errno: %d)\r\n",strerror(errno),errno);
		exit(0);		
	}

	printf("client[%s]Port[%d] connected!!!\r\n", inet_ntoa(srv_addr.sin_addr), srv_addr.sin_port);
	
	while(1)
	{

		
	}
}