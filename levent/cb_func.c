#include <event2/event.h>
#include <stdio.h>

static int n_calls = 0;

void cb_func(evutil_socket_t fd, short what, void *arg)
{
    struct event *me = arg;

    printf("cb_func called %d times so far.\n", ++n_calls);

    if (n_calls > 100)
       event_del(me);
}

void run(struct event_base *base)
{
    struct timeval one_sec = { 1, 0 };
    struct event *ev;
    struct timeval ten_sec;

  	ten_sec.tv_sec = 10;
  	ten_sec.tv_usec = 0;
  
    ev = event_new(base, -1, EV_PERSIST, cb_func, ev);
    event_add(ev, &one_sec);

    event_base_loopexit(base, &ten_sec);
    event_base_dispatch(base);
}

int main(){
	struct event_base *base = event_base_new();
	run(base);
	return 0;
}