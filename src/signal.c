#include "ft_ping.h"

static t_global g_data = {NULL, NULL, 0};

void sigint_handler(int signum) {
	(void)signum;

	if (g_data.interrupted) {
		// Double SIGINT : forcer la sortie
		if (g_data.ping && g_data.ping->sockfd >= 0) {
			close(g_data.ping->sockfd);
		}
		exit(1);
	}

	g_data.interrupted = 1;

	if (g_data.ping) {
		g_data.ping->running = 0;
	}

	// Afficher les stats
	if (g_data.ping && g_data.stats) {
		printf("\n");
		print_stats(g_data.ping, g_data.stats);
		cleanup(g_data.ping);
		exit(0);
	}
}

void sigquit_handler(int signum) {
	(void)signum;

	if (g_data.ping && g_data.stats) {
		// Sauvegarder et restaurer errno pour signal-safe
		int saved_errno = errno;

		// Calculer les statistiques actuelles
		long transmitted = g_data.stats->transmitted;
		long received = g_data.stats->received;
		double loss = 0.0;

		if (transmitted > 0) {
			loss = ((double)(transmitted - received) / transmitted) * 100.0;
		}

		if (received > 0) {
			double avg = g_data.stats->rtt_sum / received;
			printf("%ld/%ld packets, %.0f%% loss, min/avg/max = %.3f/%.3f/%.3f ms\n",
				received, transmitted, loss,
				g_data.stats->rtt_min, avg, g_data.stats->rtt_max);
		} else {
			printf("%ld/%ld packets, 100%% loss\n", received, transmitted);
		}

		errno = saved_errno;
	}
}

void setup_signal(t_ping *ping, t_stats *stats) {
	struct sigaction sa_int, sa_quit;

	g_data.ping = ping;
	g_data.stats = stats;
	g_data.interrupted = 0;

	// SIGINT (Ctrl+C)
	memset(&sa_int, 0, sizeof(sa_int));
	sa_int.sa_handler = sigint_handler;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;

	if (sigaction(SIGINT, &sa_int, NULL) < 0) {
		perror("sigaction SIGINT");
		cleanup(ping);
		exit(EXIT_FAILURE);
	}

	// SIGQUIT (Ctrl+\)
	memset(&sa_quit, 0, sizeof(sa_quit));
	sa_quit.sa_handler = sigquit_handler;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = SA_RESTART;
	// SA_RESTART == appels système interrompu auto restart

	if (sigaction(SIGQUIT, &sa_quit, NULL) < 0) {
		perror("sigaction SIGQUIT");
		cleanup(ping);
		exit(EXIT_FAILURE);
	}
}