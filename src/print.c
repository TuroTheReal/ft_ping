#include "ft_ping.h"

void print_help() {
    printf("Usage: ./ft_ping [options] <hostname>\n");
    printf("Options:\n");
    printf("  -v             Enable verbose output\n");
    printf("  -c <count>     Set the number of packets to send\n");
    printf("  -i <interval>  Set the interval between packets\n");
    printf("  -W <timeout>   Set the timeout for receiving a reply\n");
    printf("  -t <ttl>       Set the Time To Live\n");
    printf("  --ttl <ttl>    Set the Time To Live (long format)\n");
}