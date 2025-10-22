#include "ft_ping.h"

void resolve_hostname(t_ping *ping){

	struct addrinfo hints = {0};
	struct addrinfo *result;
	int status;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;

	status = getaddrinfo(ping->hostname, NULL, &hints, &result);
	if (status != 0) {
		fprintf(stderr, "ping: %s: %s\n", ping->hostname, gai_strerror(status));
		cleanup(ping);
		exit(EXIT_FAILURE);
	}

	memcpy(&ping->dest_addr, result->ai_addr, sizeof(struct sockaddr_in));

	freeaddrinfo(result);
}