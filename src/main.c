#include "ft_ping.h"

void cleanup(t_ping *ping) {
	// Fermer le socket
	if (ping->sockfd >= 0) {
		close(ping->sockfd);
		ping->sockfd = -1;
	}
}

int main(int ac, char **av) {
	t_ping ping = {
		.count = 0,
		.dest_addr = {0},
		.interval = 1.0,
		.timeout = 1.0,  // 1 seconde par défaut (comme ping standard)
		.ttl = 64,
		.running = 1,
		.pid = getpid() & 0xFFFF,
		.seq = 0,
		.sockfd = -1,
		.hostname = NULL,
		.verbose = 0
	};

	t_stats stats = {
		.transmitted = 0,
		.received = 0,
		.rtt_min = INFINITY,
		.rtt_max = 0,
		.rtt_sum = 0,
		.rtt_sq_sum = 0
	};

	parse_args(ac, av, &ping);

	// printf("======= DEBUG =======\n");
	// printf("  Hostname: %s\n", ping.hostname);
	// printf("  Verbose: %d\n", ping.verbose);
	// printf("  Count: %d\n", ping.count);
	// printf("  Interval: %.2f\n", ping.interval);
	// printf("  Timeout: %.2f\n", ping.timeout);
	// printf("  TTL: %d\n", ping.ttl);

	ping.sockfd = create_socket(&ping);
	resolve_hostname(&ping);
	setup_signal(&ping, &stats);

	do_ping(&ping, &stats);

	print_stats(&ping, &stats);
	cleanup(&ping);

	return 0;
}
