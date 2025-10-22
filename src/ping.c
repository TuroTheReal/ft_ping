#include "ft_ping.h"

// Calcul du checksum ICMP
uint16_t checksum(void *data, int len) {
	uint32_t sum = 0;
	uint16_t *ptr = (uint16_t*)data;

	// Additionner par mots de 16 bits
	while (len > 1) {
		sum += *ptr++;
		len -= 2;
	}

	// Gérer l'octet restant si longueur impaire
	if (len == 1) {
		sum += *(uint8_t*)ptr;
	}

	// Replier les retenues
	while (sum >> 16) {
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	// Complément à 1
	return (uint16_t)~sum;
}

// Créer et envoyer un paquet ICMP Echo Request
int send_ping(t_ping *ping) {
	char packet[64];  // 64 bytes standard pour ping
	struct icmp *icmp_hdr = (struct icmp *)packet;

	// Remplir le header ICMP
	memset(packet, 0, sizeof(packet));
	icmp_hdr->icmp_type = ICMP_ECHO;	  // Type 8 = Echo Request
	icmp_hdr->icmp_code = 0;
	icmp_hdr->icmp_id = htons(ping->pid);
	icmp_hdr->icmp_seq = htons(ping->seq);
	icmp_hdr->icmp_cksum = 0;  // Doit être 0 avant calcul

	// Remplir les données (optionnel, mais ping standard le fait)
	for (int i = sizeof(struct icmp); i < 64; i++) {
		packet[i] = i;
	}

	// Calculer le checksum
	icmp_hdr->icmp_cksum = checksum(packet, 64);

	// Enregistrer le timestamp de départ
	gettimeofday(&ping->tv, NULL);

	// Envoyer le paquet
	ssize_t sent = sendto(ping->sockfd, packet, 64, 0,
						  (struct sockaddr *)&ping->dest_addr,
						  sizeof(ping->dest_addr));

	if (sent < 0) {
		perror("sendto");
		return -1;
	}

	return 0;
}

// Recevoir et traiter la réponse ICMP
int receive_ping(t_ping *ping, t_stats *stats) {
	char buffer[1024];
	struct sockaddr_in src_addr;
	socklen_t addr_len = sizeof(src_addr);
	struct timeval timeout;
	fd_set read_fds;

	// Configuration du timeout pour select()
	timeout.tv_sec = (int)ping->timeout;
	timeout.tv_usec = (int)((ping->timeout - (int)ping->timeout) * 1000000);

	FD_ZERO(&read_fds);
	FD_SET(ping->sockfd, &read_fds);

	// Attendre la réponse avec timeout
	int ready = select(ping->sockfd + 1, &read_fds, NULL, NULL, &timeout);

	if (ready < 0) {
		if (ping->running) {  // Ne pas afficher erreur si CTRL+C
			perror("select");
		}
		return -1;
	}

	if (ready == 0) {
		// Timeout
		printf("Request timeout for icmp_seq %d\n", ping->seq);
		return -1;
	}

	// Recevoir le paquet
	ssize_t bytes = recvfrom(ping->sockfd, buffer, sizeof(buffer), 0,
							 (struct sockaddr *)&src_addr, &addr_len);

	if (bytes < 0) {
		perror("recvfrom");
		return -1;
	}

	// Calculer le RTT
	struct timeval now;
	gettimeofday(&now, NULL);
	double rtt = (now.tv_sec - ping->tv.tv_sec) * 1000.0 +
				 (now.tv_usec - ping->tv.tv_usec) / 1000.0;

	// Parser l'IP header
	struct ip *ip_hdr = (struct ip *)buffer;
	int ip_header_len = ip_hdr->ip_hl * 4;

	// Parser l'ICMP header
	struct icmp *icmp_hdr = (struct icmp *)(buffer + ip_header_len);

	int type = icmp_hdr->icmp_type;
	int code = icmp_hdr->icmp_code;
	int ttl = ip_hdr->ip_ttl;
	uint16_t id = ntohs(icmp_hdr->icmp_id);
	uint16_t seq = ntohs(icmp_hdr->icmp_seq);

	// Vérifier que c'est notre paquet
	if (id != ping->pid) {
		return -1;  // Pas pour nous
	}

	// Traiter selon le type ICMP
	if (type == ICMP_ECHOREPLY) {
		// Réponse normale
		stats->received++;

		// Mettre à jour les statistiques RTT
		if (rtt < stats->rtt_min) stats->rtt_min = rtt;
		if (rtt > stats->rtt_max) stats->rtt_max = rtt;
		stats->rtt_sum += rtt;
		stats->rtt_sq_sum += rtt * rtt;

		// Afficher la réponse
		char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &ip_hdr->ip_src, ip_str, sizeof(ip_str));
		printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n",
			   ip_str, seq, ttl, rtt);

		return 0;
	}
	else if (type == ICMP_TIME_EXCEEDED && ping->verbose) {
		// TTL expiré (uniquement en mode verbose)
		char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &ip_hdr->ip_src, ip_str, sizeof(ip_str));
		printf("From %s icmp_seq=%d Time to live exceeded\n", ip_str, seq);
		return -1;
	}
	else if (type == ICMP_DEST_UNREACH && ping->verbose) {
		// Destination inaccessible (uniquement en mode verbose)
		char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &ip_hdr->ip_src, ip_str, sizeof(ip_str));
		printf("From %s icmp_seq=%d Destination Unreachable (code=%d)\n",
			   ip_str, seq, code);
		return -1;
	}

	return -1;
}

void do_ping(t_ping *ping, t_stats *stats) {
	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ping->dest_addr.sin_addr, ip_str, sizeof(ip_str));

	printf("PING %s (%s) 56(84) bytes of data.\n", ping->hostname, ip_str);

	while (ping->running) {
		// Vérifier si on a atteint le count (si défini)
		if (ping->count > 0 && stats->transmitted >= ping->count) {
			break;
		}

		if (send_ping(ping) == 0) {
			stats->transmitted++;

			receive_ping(ping, stats);
		}

		// Incrémenter le numéro de séquence
		ping->seq++;

		// Attendre l'intervalle (si pas le dernier paquet)
		if (ping->running && (ping->count == 0 || stats->transmitted < ping->count)) {
			usleep((useconds_t)(ping->interval * 1000000));
		}
	}

	printf("\n");
}
