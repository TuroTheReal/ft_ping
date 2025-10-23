#include "ft_ping.h"

int create_socket(t_ping *ping)
{
	int sockfd;

	// Création du socket RAW pour ICMP (need sudo)
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	// Définir le TTL SEULEMENT si spécifié par l'utilisateur (-t/--ttl)
	if (ping->ttl > 0) {
		if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ping->ttl, sizeof(ping->ttl)) < 0) {
			perror("setsockopt (TTL)");
			close(sockfd);
			exit(EXIT_FAILURE);
		}
	}

	// Timeout pour recvfrom -W
	if (ping->timeout > 0) {
		struct timeval tv;
		tv.tv_sec = (int)ping->timeout;
		tv.tv_usec = (ping->timeout - tv.tv_sec) * 1000000;
		if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
			perror("setsockopt (timeout)");
			close(sockfd);
			exit(EXIT_FAILURE);
		}
	}

	return sockfd;
}
