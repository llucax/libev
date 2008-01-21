// make LDFLAGS=-lev CXXFLAGS="-g -Wall -I." test

#include <ev++.h>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <unistd.h>
#include <fcntl.h>

/* called when data readable on stdin */
struct myclass
{
	void io_cb (ev::io &w, int revents)
	{
		printf ("stdin ready\n");
		char buff[BUFSIZ];
		buff[0] = '\0';
		int r = read (w.fd, buff, BUFSIZ);
		printf ("%d bytes read: %s", r, buff);
		w.stop (); /* just a syntax example */
		// same as w.loop ().unloop (ev::ONE); in this example
		ev::default_loop.unloop (); /* leave one loop call */
	}
	void
	timeout_cb (ev::timer &w, int revents)
	{
		printf ("timeout!\n");
		// simulate activity on STDIN_FILENO
		w.loop.feed_fd_event (0, ev::READ);
	}
};

int
main (void)
{
	ev::default_loop.init (ev::NOENV);
        ev::loop_ref loop = ev::default_loop;

	int flags;
	if ((flags = fcntl (0, F_GETFL, 0)) < 0)
		perror ("fcntl F_GETFL");
	if (fcntl (0, F_SETFL, flags | O_NONBLOCK) < 0)
		perror ("fcntl F_SETFL");

	myclass obj;

	// io event
	ev::io iow;

	iow.set <myclass, &myclass::io_cb> (&obj);

	/* start watching fd 0 for reading (default) */
	iow.start (/*STDIN_FILENO*/ 0, ev::READ);

	// timeout event
	ev::timer tw (ev::default_loop); // just to test
	tw.set <myclass, &myclass::timeout_cb> (&obj);

	/* simple non-repeating 5.5 second timeout */
	tw.start (5.5);

	/* loop till timeout or data ready */
        ev::default_loop.loop ();

	assert (loop == ev::default_loop);
	assert (loop == ev_default_loop (0));
	ev::dynamic_loop l;
	assert (loop != l);
	ev::loop_ref lr(l);
	lr = ev_default_loop (0);
	iow.loop = ev_default_loop (0);
	ev::delay (.5);

	return 0;
}


