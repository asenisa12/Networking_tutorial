#include "../lib/unp.h"
#include "../lib/error.c"
#include "../lib/readline.c"
#include "../lib/wrapsock.c"




void str_cli(FILE *fp, int sockfd){
	char sendline[MAXLINE], recivline[MAXLINE];

	while (fgets(sendline,MAXLINE,fp)!=NULL){
		write(sockfd, sendline, strlen(sendline));

		if(readline(sockfd,recivline,MAXLINE)==0)
			err_quit("str_cli: server terminated prematurely");

		fputs(recivline,stdout);
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