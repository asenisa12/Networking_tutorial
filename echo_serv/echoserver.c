#include "../lib/unp.h"
#include "../lib/signal.c"



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


void sig_child(int signo){

	pid_t pid;
	int stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d was terminated\n",pid);

	return;
}


int main(){

	int listenfd, connfd;
	pid_t childpid;
	socklen_t client;

	struct sockaddr_in clientaddr, servaddr;

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0))<0)
		perror("socket:");


	bzero(&servaddr,sizeof(servaddr));

	printf("%d\n",SERV_PORT);
	servaddr.sin_family =AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);


	if( (bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))<0)
		perror("bind:");


	if((listen(listenfd,100))<0)
		perror("listen:");


	signal(SIGCHLD, sig_child);

	for(;;){
		client = sizeof(clientaddr);
		connfd = accept(listenfd,(struct sockaddr*)&clientaddr,&clientaddr);

		if ((childpid = fork()) ==0){

			close(listenfd);

			

			str_echo2(connfd);

			write(connfd,"asen e pich\n",strlen("asen e pich\n"));
			
			exit(0);
		}

		close(connfd);
	}

	return 0;

}
	
