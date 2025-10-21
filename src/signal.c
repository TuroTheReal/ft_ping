#include "ft_ping.h"

static t_ping *g_ping = NULL;

void signal_handler(int signum) {
	(void)signum;

	if (g_ping) {
		g_ping->running = 0;
	}
}

void setup_signal(t_ping *ping) {
	struct sigaction sa;

	g_ping = ping;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) < 0) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
}