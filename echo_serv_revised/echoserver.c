#include "../lib/unp.h"
#include "../lib/signal.c"
#include "../lib/error.c"
#include "../lib/wrapsock.c"

#include <poll.h>
#define OPEN_MAX 256


void str_echo2(int sockfd){
	ssize_t n;
	char buf[MAXLINE];

again:
	while( (n=read(sockfd, buf, MAXLINE)) > 0)
		write(sockfd, buf, n);

	if(n < 0 && errno ==EINTR)
		goto again;
	else if(n<0)
		perror("read error");
}




int main(){

	int i,maxi,listenfd, connfd,sockfd;
	socklen_t clien;
	int nready;
	ssize_t n;
	char buf[MAXLINE];
	struct pollfd client[OPEN_MAX];

	struct sockaddr_in cliaddr, servaddr;

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("socket:");
		return -1;
	}


	bzero(&servaddr,sizeof(servaddr));

	printf("%d\n",SERV_PORT);
	servaddr.sin_family =AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);


	if( (bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))<0){
		perror("bind:");
		return -1;
	}

	if((listen(listenfd,100))<0){
		perror("listen:");
		return -1;
	}

	client[0].fd = listenfd;
	client[0].events = POLLRDNORM;

	for (i=1;i< OPEN_MAX; i++)
		client[i].fd=-1;
	maxi=0;

	for (;;){
		nready = poll(client, maxi+1, INFTIM);

		if (client[0].revents & POLLRDNORM){
			clien = sizeof(cliaddr);
			connfd =Accept(listenfd, (struct sockaddr *)&cliaddr, &clien);
		


			for (i=1; i < OPEN_MAX; i++)
				if (client[i].fd < 0){
					client[i].fd = connfd;
					break;
				}

			if (i==OPEN_MAX)
				err_quit("too many clients");

			client[i].events = POLLRDNORM;

			if(i>maxi)
				maxi = i;
		}

		for (i=1; i <=maxi; i++){
			if((sockfd = client[i].fd) < 0)
				continue;

			if (client[i].revents & (POLLRDNORM | POLLERR)){
				if ((n = read(sockfd, buf, MAXLINE))<0){
					if (errno == ECONNRESET){
						close(sockfd);
						client[i].fd = -1;
					} else
						err_sys("read error");

				}else if (n==0){
					close(sockfd);
					client[i].fd = -1;
				} else
					write(sockfd, buf, n);
				if(--nready <=0)
					break;

			}

		}


	}


	return 0;

}
	
