#include "ft_ping.h"

static t_global g_data = {NULL, NULL, 0};

void signal_handler(int signum) {
	(void)signum;

	if (g_data.interrupted) {
		// Double SIGINT : forcer la sortie immédiate
		if (g_data.ping && g_data.ping->sockfd >= 0) {
			close(g_data.ping->sockfd);
		}
		exit(1);
	}

	g_data.interrupted = 1;

	if (g_data.ping) {
		g_data.ping->running = 0;
	}

	// Afficher les stats immédiatement
	if (g_data.ping && g_data.stats) {
		printf("\n");
		print_stats(g_data.ping, g_data.stats);
		cleanup(g_data.ping);
		exit(0);
	}
}

void setup_signal(t_ping *ping, t_stats *stats) {
	struct sigaction sa;

	g_data.ping = ping;
	g_data.stats = stats;
	g_data.interrupted = 0;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) < 0) {
		perror("sigaction");
		cleanup(ping);
		exit(EXIT_FAILURE);
	}
}
