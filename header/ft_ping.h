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
    int verbose;
    int running; // 1 par defaut

    // Bonus
    int count;           // -c (0 = infini)
    double interval;     // -i (1.0 par défaut)
    double timeout;      // -W (1.0 par défaut)
    int ttl;             // -t / --ttl (64 par défaut)

    // Données internes
    int sockfd;          // Socket descriptor
    struct sockaddr_in dest_addr;  // Adresse destination
    uint16_t pid;        // Process ID (pour l'ICMP ID)
    uint16_t seq;        // Numéro de séquence

	struct timeval tv;
} t_ping;

typedef struct s_stats {
    int transmitted;
    int received;
    double rtt_min;
    double rtt_max;
    double rtt_sum;      // Pour calculer avg
    double rtt_sq_sum;   // Pour calculer mdev (écart-type)
} t_stats;


void	parse_args(int argc, char** argv, t_ping *ping);
void	print_help();
int		create_socket(t_ping *ping);
void	resolve_hostname(t_ping *ping);
void	setup_signal(t_ping *ping);

// checksum ?
// -? ?
// FQDN ?
// tcpdump ?
// icmp ?


// man 7 raw      # Raw sockets
// man 7 icmp     # ICMP protocol
// man 2 socket   # socket() syscall

// Wikipedia ICMP (section "Echo request/reply") : https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
// Retiens :

// ICMP = protocole couche 3 (réseau)
// Type 8 = Echo Request (ce qu'on envoie)
// Type 0 = Echo Reply (ce qu'on reçoit)
// Structure : [Type][Code][Checksum][ID][Seq][Data]


// algo checksum

// uint16_t checksum(void *data, int len) {
//     uint32_t sum = 0;              // 32 bits pour capturer retenues
//     uint16_t *ptr = (uint16_t*)data; // Pointeur sur mots de 16 bits

//     // ÉTAPE 1: Additionner par mots de 16 bits
//     while (len > 1) {
//         sum += *ptr++;   // Ajoute 2 octets (16 bits)
//         len -= 2;        // Décrémente de 2 OCTETS
//     }

//     // ÉTAPE 2: Gérer l'octet restant (si longueur impaire)
//     if (len == 1) {
//         sum += *(uint8_t*)ptr;  // Dernier octet = 8 bits
//     }

//     // ÉTAPE 3: Replier les retenues (carry)
//     // Tant qu'il y a des bits au-delà des 16 premiers bits
//     while (sum >> 16) {
//         sum = (sum & 0xFFFF)  // 16 bits bas
//             + (sum >> 16);     // + retenue (bits hauts)
//     }

//     // ÉTAPE 4: Complément à 1 (inverser tous les bits)
//     return (uint16_t)~sum;
// }

// // UTILISATION:
// struct icmp *icmp = ...;
// icmp->icmp_cksum = 0;  // ⚠️ IMPORTANT: mettre à 0 avant calcul !
// icmp->icmp_cksum = checksum(icmp, packet_size);



// RTT
// struct timeval start, end;
// gettimeofday(&start, NULL);  // Avant sendto()
// // ... envoyer paquet ...
// gettimeofday(&end, NULL);    // Après recvfrom()

// // Calculer RTT en millisecondes
// double rtt = (end.tv_sec - start.tv_sec) * 1000.0 +
//              (end.tv_usec - start.tv_usec) / 1000.0;
// ```

// **Statistiques finales :**
// - `min` : plus petit RTT
// - `max` : plus grand RTT
// - `avg` : moyenne (`rtt_sum / received`)
// - `mdev` : écart-type (standard deviation)
// ```
// mdev = sqrt((sum_of_squares / n) - (avg * avg))


// POUR VERBOSE, -v
// # TTL expiré (Type 11, Code 0)
// From 192.168.1.1: icmp_seq=1 Time to live exceeded

// # Destination unreachable (Type 3)
// From 192.168.1.1: Destination Host Unreachable

// int ttl = 1;  // Va expirer au 1er routeur
// setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));



// debug command
// # Verbose (affiche plus de détails)
// sudo tcpdump -i eth0 icmp -vv

// # Afficher en hexadécimal
// sudo tcpdump -i eth0 icmp -X

// # Filtrer par host
// sudo tcpdump -i eth0 icmp and host 8.8.8.8

// # Sauvegarder dans un fichier (pour Wireshark)
// sudo tcpdump -i eth0 icmp -w capture.pcap

// Whireshark




// parsing icmp exemple

// #include <netinet/ip.h>
// #include <netinet/ip_icmp.h>

// void handle_reply(char *buffer, int bytes) {
//     // 1. Parser l'IP header
//     struct ip *ip_hdr = (struct ip *)buffer;
//     int ip_header_len = ip_hdr->ip_hl * 4;  // Longueur variable !

//     // 2. Parser l'ICMP header (juste après l'IP)
//     struct icmp *icmp_hdr = (struct icmp *)(buffer + ip_header_len);

//     // 3. Extraire les infos
//     int type = icmp_hdr->icmp_type;
//     int code = icmp_hdr->icmp_code;
//     int ttl = ip_hdr->ip_ttl;
//     uint16_t id = ntohs(icmp_hdr->icmp_id);
//     uint16_t seq = ntohs(icmp_hdr->icmp_seq);

//     // 4. Traiter selon le type
//     if (type == ICMP_ECHOREPLY) {
//         // Réponse normale
//         printf("64 bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n",
//                inet_ntoa(ip_hdr->ip_src), seq, ttl, rtt);
//     }
//     else if (type == ICMP_TIME_EXCEEDED && ping.verbose) {
//         // TTL expiré (pour -v)
//         printf("From %s icmp_seq=%d Time to live exceeded\n",
//                inet_ntoa(ip_hdr->ip_src), seq);
//     }
//     else if (type == ICMP_DEST_UNREACH && ping.verbose) {
//         // Destination inaccessible (pour -v)
//         printf("From %s icmp_seq=%d Destination Unreachable\n",
//                inet_ntoa(ip_hdr->ip_src), seq);
//     }
// }