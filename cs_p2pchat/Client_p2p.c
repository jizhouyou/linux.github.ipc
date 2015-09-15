#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m) \
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	}while(0)

void parent_handler(int signal);
int main(void)
{
	int clientsockfd;
	if(( clientsockfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
	{
		ERR_EXIT("socket");
	}
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(connect(clientsockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) <0 )
	{
		ERR_EXIT("connect");
	}
	pid_t pid;
	pid = fork();
	if(pid == -1)
	{
		ERR_EXIT("fork");
	}
	if(0 == pid)
	{
		char recvbuf[200]= {0};
		while(1)
		{
			memset(recvbuf,0,sizeof(recvbuf));
			int ret = read(clientsockfd,recvbuf,sizeof(recvbuf));
			
			if(-1 == ret)
			{
				ERR_EXIT("read");
			}
			else if(0 == ret)
			{
				printf("peer closed \n");
				break;
				
			}
			fputs(recvbuf,stdout);
			
		}
		close(clientsockfd);
		kill(getppid(),SIGUSR1);
		exit(EXIT_SUCCESS);	
	}
	else
	{
		char sendbuf[200];
		signal(SIGUSR1,parent_handler);
		while(fgets(sendbuf,sizeof(sendbuf),stdin)!=NULL)
		{
			write(clientsockfd,sendbuf,strlen(sendbuf));
			memset(sendbuf,0,sizeof(sendbuf));
			
		}
		close(clientsockfd);
		
	}
	return 0;
}	


void parent_handler(int signal)
{
	printf("client child recv a signal=%d\n",signal);
	exit(EXIT_SUCCESS);
}
