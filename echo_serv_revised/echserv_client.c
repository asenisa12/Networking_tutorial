#include "../lib/unp.h"
#include "../lib/error.c"
#include "../lib/readline.c"
#include "../lib/wrapsock.c"


void str_cli(FILE *fp, int sockfd){
	char buf[MAXLINE];
	int maxfdp1, stdineof;
	fd_set rset;
	int n;

	stdineof=0;
	FD_ZERO(&rset);

	for(;;){
		if(stdineof==0)
			FD_SET(fileno(fp),&rset);
		FD_SET(sockfd,&rset);
		maxfdp1=max(fileno(fp),sockfd)+1;
		select(maxfdp1,&rset,NULL,NULL,NULL);
		
		if(FD_ISSET(sockfd,&rset)){
			if((n=read(sockfd,buf, MAXLINE))==0){
				if(stdineof==1)
					return;
				else err_quit("server TERminated !!!");
			}
			write(fileno(stdout),buf,n);
		}

		if(FD_ISSET(fileno(fp),&rset)){
			if((n=read(fileno(fp),buf,MAXLINE))==0){
				stdineof=1;
				shutdown(sockfd,SHUT_WR);
				continue;
			}
			write(sockfd,buf,n);
		}

	}
}


int main(int argc, char **argv){

	int sockfd;
	struct sockaddr_in servaddr;

	if(argc !=2)
		err_quit("add ip addres only");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);

	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

	Connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

	str_cli(stdin, sockfd);


	return 0;
}