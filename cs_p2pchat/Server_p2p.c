
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include<signal.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


void do_service(int sockfdtemp);
void child_exit_handler(int signal);
#define ERR_EXIT(m) \
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	}while(0)\

int main(void)
{
	int listenfd;
	if((listenfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
	{
		ERR_EXIT("socket");
	}
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int on =1;//local address (port) reuse
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
	{
		ERR_EXIT("setsocketopt");
		
	}
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) <0)
	{
		ERR_EXIT("bind");
	}
	if(listen(listenfd,SOMAXCONN) < 0)
	{
		ERR_EXIT("listen");
	}
	struct sockaddr_in clientaddr;
	socklen_t clientlen = sizeof(clientaddr);
	int conn;
	pid_t servicepid ;
	do
	{
		if((conn = accept(listenfd , (struct sockaddr *) &clientaddr,&clientlen)) < 0)
		{
			ERR_EXIT("accept");
		}
		printf("ip=%s port=%d\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
		servicepid = fork();
		if(servicepid == -1)
		{
			ERR_EXIT("fork");
		}

		if(servicepid ==0)//subprocess processing the connect
		{
			signal(SIGUSR1,	child_exit_handler);	
			char sendbuf[200];
			while(fgets(sendbuf,sizeof(sendbuf),stdin) != NULL)
			{
				write(conn,sendbuf,strlen(sendbuf));
				memset(sendbuf,0,sizeof(sendbuf));
			}
			printf("server child close\n");	
			exit(EXIT_SUCCESS);
		}
		else
		{
			char recvbuf[200];
			while(1)
			{
				memset(recvbuf,0,sizeof(recvbuf));
				int ret = read(conn,recvbuf,sizeof(recvbuf));
				if(ret == -1)
				{
					ERR_EXIT("read");
				}
				else if(0 == ret)
				{
				
					printf("peer close\n");
					break;
				}
				fputs(recvbuf,stdout);
			}
			printf("server parent close\n");
			kill(servicepid,SIGUSR1);
			exit(EXIT_SUCCESS);	
		}

	}while(0);
	close(listenfd);
}
void child_exit_handler(int signal)
{
	printf("server child recv a sig = %d",signal);
	ERR_EXIT("server_child exit");
}
