#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

typedef struct packet_lag
{
	size_t len;
	char buf[200];
}packet_type;

ssize_t read_n(int fd, void *buf, size_t count);
ssize_t write_n(int fd, const void *buf, size_t count);

int main(void)
{
	int clientsock;
	if(( clientsock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0)
	{
		ERR_EXIT("socket");
	}
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5188);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(connect(clientsock,(struct sockaddr *)&servaddr,sizeof(servaddr)) <0 )
	{
		ERR_EXIT("connect");
	}
	char sendbuf[200]={0};
	char recvbuf[256]={0};
	while(fgets(sendbuf,sizeof(sendbuf),stdin)!=NULL)
	{
		write(clientsock,sendbuf,strlen(sendbuf));
		read(clientsock,recvbuf,sizeof(recvbuf));
		fputs(recvbuf,stdout);
		memset(recvbuf,0,sizeof(recvbuf));
		memset(sendbuf,0,sizeof(sendbuf));
	}
	close(clientsock);
	return 0;
}	


ssize_t read_n(int fd, void *buf, size_t count)
{
	size_t nleft = count;
	ssize_t nread;
	char *bufp = (char*)buf;
	while(nleft >0)
	{
		if((nread = read(fd,bufp,nleft)) < 0)
		{
			if(errno == EINTR)
			{
				continue;
			}
			return -1;
		}
		else if(0 == nread)
		{
			return (count - nleft);
		}
		bufp += nread;
		nleft -= nread;
	}
	return count;
}
ssize_t write_n(int fd, const void *buf, size_t count)
{
	size_t nleft = count;
	ssize_t nwritten;
	char *bufp = (char *) buf;
	while(nleft > 0)
	{

	}
}
