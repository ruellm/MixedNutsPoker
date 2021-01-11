#if !defined(WIN32)

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int
daemon(int nochdir, int noclose)
{
	int maxfd, fd = 0;

	if (getppid() == 1)
		return 0;		/* run_as_daemon has already been called. */

	switch (fork())
	{
		case 0: /* The child process. */
			break;
		case -1: /* Fork failed. */
			return -1;
		default: /* Exit the parent. */
			_exit(0);
	}
	/* Now running the child (daemon). */

	if (setsid() < 0) /* Obtain a new process group. */
		return -1;

	switch (fork())
	{
		case 0:
			break;
		case -1:
			return -1;
		default:
			_exit(0);
	}


	if (!nochdir)
		chdir("/"); /* Change working directory. */

	if (!noclose)
	{
		/* Close all open handles. */
		maxfd = sysconf(_SC_OPEN_MAX);
		while (fd < maxfd)
			close(fd++);

		/* Use /dev/null as stdin/stdout/stderr. */
		open("/dev/null", O_RDWR);
		dup(0);
		dup(0);
	}
	
	return 0;
}

#endif
