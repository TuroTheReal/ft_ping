#ifndef FT_PING_H
#define FT_PING_H

#include <arpa/inet.h> // inet pton & ntop
#include <ctype.h> // is*
#include <getopt.h> // getopt()
#include <math.h> // INFINITY
#include <netdb.h>  // Pour getaddrinfo, struct addrinfo, gai_strerror
#include <netinet/ip.h> // IPPROTO_IP
#include <netinet/ip_icmp.h> // ICMP_ECHO, ICMP_ECHOREPLY
#include <signal.h>  // Pour sigaction, SIGINT
#include <stdint.h> // uint8_t, uint16_t, uint32_t
#include <stdio.h> // printf(), fprintf(), perror()
#include <stdlib.h> // exit(), atoi(), strtol()
#include <string.h> // memset(), memcpy(), strcmp(), strlen()
#include <unistd.h> // getopt(), read(), write(), close(), usleep()
#include <sys/types.h> // id_t, size_t, ssize_t
#include <sys/time.h> //timeval
#include <sys/socket.h> // socket(), setsockopt(), sendto(), recvfrom()

typedef struct s_ping {
	// Mandatory
	char *hostname;
	int verbose; // -v OK
	int running; // 1 par defaut

	// Bonus
	int count;			// -c (0 = infini) OK
	double interval;	// -i (1.0 par défaut) OK
	double timeout;		// -W (1.0 par défaut) a faire
	int ttl;			// -t / --ttl (64 par défaut) a finir

	// Données internes
	int sockfd;			// Socket descriptor
	struct sockaddr_in dest_addr;  // Adresse destination
	uint16_t pid;		// Process ID (pour l'ICMP ID)
	uint16_t seq;		// Numéro de séquence

	struct timeval tv;	// Temps du dernier paquet envoyé
	struct timeval start_time;
} t_ping;

typedef struct s_stats {
	int transmitted;
	int received;
	double rtt_min;
	double rtt_max;
	double rtt_sum;		// Pour calculer avg
	double rtt_sq_sum;	// Pour calculer mdev (écart-type)
} t_stats;

// Structure globale pour signal + stats
typedef struct s_global {
	t_ping *ping;
	t_stats *stats;
	int interrupted;
} t_global;

void	parse_args(int argc, char** argv, t_ping *ping);
void	print_help();
void	print_version();
void	print_stats(t_ping *ping, t_stats *stats);
int		create_socket(t_ping *ping);
void	resolve_hostname(t_ping *ping);
void	setup_signal(t_ping *ping, t_stats *stats);
void	do_ping(t_ping *ping, t_stats *stats);
void	cleanup(t_ping *ping);

#endif