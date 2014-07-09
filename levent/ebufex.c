#include <event2/event.h>
#include <event2/bufferevent.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>


void eventcb(struct bufferevent *bev, short events, void *ptr){

	if (events & BEV_EVENT_CONNECTED){
		printf("connected\n");
	}else
		perror("connection error");

}

int main(){
	struct event_base *base = event_base_new();
	struct bufferevent *bev;
	struct sockaddr_in sin;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port = htons(9877);

	bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, NULL, NULL, eventcb, NULL);

	if(bufferevent_socket_connect(bev, 
		(struct sockaddr*)&sin,sizeof(sin)) <0){
		bufferevent_free(bev);
		return -1;
	}

	event_base_dispatch(base);
	return 0;
}