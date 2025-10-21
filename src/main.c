#include "ft_ping.h"

int main(int ac, char **av){
	t_ping ping = {
		.count = 0,
		.dest_addr = {0},
		.interval = 1.0,
		.timeout = 0,
		.ttl = 64,
		.running = 1,
		.pid = getpid() & 0xFFFF,
		.seq = 0
	};

	t_stats stats = {
		.transmitted = 0,
		.received = 0,
		.rtt_min = INFINITY,  // Important !
		.rtt_max = 0,
		.rtt_sum = 0,
		.rtt_sq_sum = 0
	};
	(void)stats;

	parse_args(ac, av, &ping);

	printf("======= DEBUG =======\n");
    printf("  Hostname: %s\n", ping.hostname);
    printf("  Verbose: %d\n", ping.verbose);
    printf("  Count: %d\n", ping.count);
    printf("  Interval: %.2f\n", ping.interval);
    printf("  Timeout: %.2f\n", ping.timeout);
    printf("  TTL: %d\n", ping.ttl);

	ping.sockfd = create_socket(&ping);

	resolve_hostname(&ping);

	setup_signal(&ping);
	// do_ping(&ping, &stats);

	// print_stats(&ping, &stats);

	return 0;
}