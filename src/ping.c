#include "ft_ping.h"

// Calcul du checksum ICMP
uint16_t checksum(void *data, int len) {
	uint32_t sum = 0;
	uint16_t *ptr = (uint16_t*)data;

	while (len > 1) {
		sum += *ptr++;
		len -= 2;
	}

	if (len == 1) {
		sum += *(uint8_t*)ptr;
	}

	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	return (uint16_t)~sum;
}

int send_ping(t_ping *ping) {
	char packet[64];
	struct icmp *icmp_hdr = (struct icmp *)packet;

	memset(packet, 0, sizeof(packet));
	icmp_hdr->icmp_type = ICMP_ECHO;
	icmp_hdr->icmp_code = 0;
	icmp_hdr->icmp_id = htons(ping->pid);
	icmp_hdr->icmp_seq = htons(ping->seq);
	icmp_hdr->icmp_cksum = 0;

	for (int i = sizeof(struct icmp); i < 64; i++) {
		packet[i] = i;
	}

	icmp_hdr->icmp_cksum = checksum(packet, 64);
	gettimeofday(&ping->tv, NULL);

	ssize_t sent = sendto(ping->sockfd, packet, 64, 0,
						  (struct sockaddr *)&ping->dest_addr,
						  sizeof(ping->dest_addr));

	if (sent < 0) {
		perror("sendto");
		return -1;
	}

	return 0;
}

int receive_ping(t_ping *ping, t_stats *stats) {
	char buffer[1024];
	struct sockaddr_in src_addr;
	socklen_t addr_len = sizeof(src_addr);
	struct timeval deadline, now, timeout;
	fd_set read_fds;

	// Calculer le temps limite (deadline)
	double wait_time = (ping->timeout > 0) ? ping->timeout : ping->interval;
	gettimeofday(&deadline, NULL);
	deadline.tv_sec += (int)wait_time;
	deadline.tv_usec += (int)((wait_time - (int)wait_time) * 1000000);
	if (deadline.tv_usec >= 1000000) {
		deadline.tv_sec++;
		deadline.tv_usec -= 1000000;
	}

	// Boucle pour ignorer les paquets qui ne correspondent pas
	while (ping->running) {
		// Calculer le temps restant avant deadline
		gettimeofday(&now, NULL);

		timeout.tv_sec = deadline.tv_sec - now.tv_sec;
		timeout.tv_usec = deadline.tv_usec - now.tv_usec;

		if (timeout.tv_usec < 0) {
			timeout.tv_sec--;
			timeout.tv_usec += 1000000;
		}

		// Si le timeout est dépassé
		if (timeout.tv_sec < 0) {
			printf("Request timeout for icmp_seq %d\n", ping->seq);
			return -1;
		}

		FD_ZERO(&read_fds);
		FD_SET(ping->sockfd, &read_fds);

		int ready = select(ping->sockfd + 1, &read_fds, NULL, NULL, &timeout);

		if (ready < 0) {
			if (ping->running) {
				perror("select");
			}
			return -1;
		}

		if (ready == 0) {
			printf("Request timeout for icmp_seq %d\n", ping->seq);
			return -1;
		}

		ssize_t bytes = recvfrom(ping->sockfd, buffer, sizeof(buffer), 0,
								 (struct sockaddr *)&src_addr, &addr_len);

		if (bytes < 0) {
			if (ping->running) {
				perror("recvfrom");
			}
			return -1;
		}

		struct ip *ip_hdr = (struct ip *)buffer;
		int ip_header_len = ip_hdr->ip_hl * 4;
		struct icmp *icmp_hdr = (struct icmp *)(buffer + ip_header_len);

		int type = icmp_hdr->icmp_type;
		int code = icmp_hdr->icmp_code;
		int ttl = ip_hdr->ip_ttl;
		uint16_t id = ntohs(icmp_hdr->icmp_id);
		uint16_t seq = ntohs(icmp_hdr->icmp_seq);

		// Vérifier que c'est bien NOTRE paquet (bon PID)
		if (id != ping->pid) {
			continue;  // Ignorer, c'est un autre processus ping
		}

		// Vérifier la séquence
		if (seq != ping->seq) {
			continue;  // Ignorer, c'est une vieille réponse ou un duplicata
		}

		// On a reçu le bon paquet !
		struct timeval recv_time;
		gettimeofday(&recv_time, NULL);

		double rtt = (recv_time.tv_sec - ping->tv.tv_sec) * 1000.0 +
					 (recv_time.tv_usec - ping->tv.tv_usec) / 1000.0;

		if (type == ICMP_ECHOREPLY) {
			stats->received++;

			if (rtt < stats->rtt_min) stats->rtt_min = rtt;
			if (rtt > stats->rtt_max) stats->rtt_max = rtt;
			stats->rtt_sum += rtt;
			stats->rtt_sq_sum += rtt * rtt;

			char ip_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &ip_hdr->ip_src, ip_str, sizeof(ip_str));
			printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n",
				   ip_str, seq, ttl, rtt);

			return 0;
		}
		else if (type == ICMP_TIME_EXCEEDED && ping->verbose) {
			char ip_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &ip_hdr->ip_src, ip_str, sizeof(ip_str));
			printf("From %s icmp_seq=%d Time to live exceeded\n", ip_str, seq);
			return -1;
		}
		else if (type == ICMP_DEST_UNREACH && ping->verbose) {
			char ip_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &ip_hdr->ip_src, ip_str, sizeof(ip_str));
			printf("From %s icmp_seq=%d Destination Unreachable (code=%d)\n",
				   ip_str, seq, code);
			return -1;
		}
	}

	return -1;
}

void do_ping(t_ping *ping, t_stats *stats) {
	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ping->dest_addr.sin_addr, ip_str, sizeof(ip_str));

	printf("PING %s (%s) 56(84) bytes of data.\n", ping->hostname, ip_str);

	while (ping->running) {
		if (ping->count > 0 && stats->transmitted >= ping->count) {
			break;
		}

		if (send_ping(ping) == 0) {
			stats->transmitted++;
			receive_ping(ping, stats);
		}

		ping->seq++;

		if (ping->running && (ping->count == 0 || stats->transmitted < ping->count)) {
			usleep((useconds_t)(ping->interval * 1000000));
		}
	}

	if (ping->running) {
		printf("\n");
	}
}