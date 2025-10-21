#include "ft_ping.h"

int create_socket(t_ping *ping)
{
	int sockfd;
	int ttl = ping->ttl;

	// Création du socket RAW pour ICMP (nécessite sudo)
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		perror("ft_ping: socket");
		exit(EXIT_FAILURE);
	}

	// Définir le TTL (Time To Live)
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
		perror("ft_ping: setsockopt (TTL)");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	// Timeout pour recvfrom (optionnel, utile plus tard)
	if (ping->timeout > 0) {
		struct timeval tv;
		tv.tv_sec = (int)ping->timeout;
		tv.tv_usec = (ping->timeout - tv.tv_sec) * 1000000;
		if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
			perror("ft_ping: setsockopt (timeout)");
			close(sockfd);
			exit(EXIT_FAILURE);
		}
	}

	return sockfd;
}

int main(int ac, char **av){
	t_ping ping = {
		.count = 0,
		.dest_addr = {0},
		.interval = 0,
		.timeout = 0,
		.ttl = 64,
		.running = 1
	};
	t_stats stats = {0};
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

	// resolve_hostname(&ping);

	// setup_signal(&ping);
	// do_ping(&ping, &stats);

	// print_stats(&ping, &stats);

	return 0;
}