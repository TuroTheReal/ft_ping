#include "ft_ping.h"

void	print_version(){
	printf("ping from Arthur Bernard 20251026\n");
}

void print_help() {
	printf("Usage: ./ft_ping [options] <hostname>\n");
	printf("Options:\n");
	printf("  -c, --count=NUMBER		stop after sending NUMBER packets\n");
	printf("  -i, --interval=NUMBER		wait NUMBER seconds between sending each packet\n");
	printf("  -v, --verbose			verbose output\n");
	printf("  -V, --version				print program version\n");
	printf("  -t, --ttl=N			specify N as time-to-live\n");
	printf("  -w, --timeout=N		number of seconds to wait for response\n");
	printf("  -?, -h, --help		give this help list\n");
}

void print_stats(t_ping *ping, t_stats *stats) {
	printf("--- %s ping statistics ---\n", ping->hostname);

	// Calculer le pourcentage de perte
	int packet_loss = 0;
	if (stats->transmitted > 0) {
		packet_loss = (int)(((stats->transmitted - stats->received) * 100.0) / stats->transmitted);
	}

	// Calculer temps total depuis le DÉBUT
	struct timeval end_time, diff;
	gettimeofday(&end_time, NULL);
	timersub(&end_time, &ping->start_time, &diff);
	long total_ms = diff.tv_sec * 1000 + diff.tv_usec / 1000;

	if (packet_loss)
		printf("%d packets transmitted, %d received, +%d errors, %d%% packet loss, time %ldms\n",
			stats->transmitted, stats->received,stats->transmitted - stats->received, packet_loss, total_ms);
	else
		printf("%d packets transmitted, %d received, %d%% packet loss, time %ldms\n",
			stats->transmitted, stats->received, packet_loss, total_ms);

	// Afficher les stats RTT si on a reçu au moins un paquet
	if (stats->received > 0) {
		double avg = stats->rtt_sum / stats->received;

		// Calculer mdev (écart-type)
		double variance = (stats->rtt_sq_sum / stats->received) - (avg * avg);
		double mdev = sqrt(variance > 0 ? variance : 0);

		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
			stats->rtt_min, avg, stats->rtt_max, mdev);
	}
}
