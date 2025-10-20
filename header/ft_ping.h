// Dans ta structure, prévois juste les champs :
typedef struct s_ping {
    // Mandatory
    char    *hostname;
    int     verbose;        // -v
    int     running;

    // Bonus (mettre à 0/NULL au début, implémenter plus tard)
    // int     count;          // -c (0 = infini)
    // double  interval;       // -i (1.0 par défaut)
    // double  timeout;        // -W (1.0 par défaut)
} t_ping;

typedef struct s_stats {
    int     transmitted;    // Compteur
    int     received;       // Compteur
    double  rtt_min;        // Simple comparaison
    double  rtt_max;        // Simple comparaison
    double  rtt_sum;        // Accumulation
    double  rtt_sq_sum;     // Accumulation pour mdev
} t_stats;

checksum ?
-? ?
FQDN ?
tcpdump ?
icmp ?



man 7 raw      # Raw sockets
man 7 icmp     # ICMP protocol
man 2 socket   # socket() syscall

🔗 Lis juste ça :

Wikipedia ICMP (section "Echo request/reply") : https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
Retiens :

ICMP = protocole couche 3 (réseau)
Type 8 = Echo Request (ce qu'on envoie)
Type 0 = Echo Reply (ce qu'on reçoit)
Structure : [Type][Code][Checksum][ID][Seq][Data]