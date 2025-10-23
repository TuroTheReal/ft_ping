#include "ft_ping.h"

void	print_version(){
	printf("ping from Arthur Bernard 20251026\n");
}

void print_help() {
	printf("Usage: ./ft_ping [options] <hostname>\n");
	printf("Options:\n");
	printf("  -v			Enable verbose output\n");
	printf("  -c <count>	Set the number of packets to send\n");
	printf("  -i <interval> Set the interval between packets\n");
	printf("  -W <timeout>  Set the timeout for receiving a reply\n");
	printf("  -t <ttl>	 	Set the Time To Live\n");
	printf("  --ttl <ttl>  	Set the Time To Live (long format)\n");
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
