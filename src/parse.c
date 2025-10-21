#include "ft_ping.h"

void parse_args(int argc,char** argv,t_ping *ping){

	int opt;

	static struct option long_options[] = {
		{"ttl", required_argument, NULL, 't'},
		{0, 0, 0, 0}
	};

    while ((opt = getopt_long(argc, argv, "v?c:i:W:t:", long_options, NULL)) != -1) {
		switch (opt) {
			case 'v': ping->verbose = 1; break;
			case 'c': {
				char *endptr;
				ping->count = strtol(optarg, &endptr, 10);
				if (*endptr != '\0' || ping->count <= 0) {
					fprintf(stderr, "ping: invalid argument: '%s'\n", optarg);
					exit(1);
				}
				break;
			}
			case 'i': {
				char *endptr;
				ping->interval = strtod(optarg, &endptr);
				if (*endptr != '\0' || ping->interval <= 0) {
					fprintf(stderr, "ping: invalid argument: '%s'\n", optarg);
					exit(1);
				}
				break;
			}

			case 'W': {
				char *endptr;
				ping->timeout = strtod(optarg, &endptr);
				if (*endptr != '\0' || ping->timeout < 0) {
					fprintf(stderr, "ping: invalid argument: '%s'\n", optarg);
					exit(1);
				}
				break;
			}

			case 't': {
				char *endptr;
				ping->ttl = strtol(optarg, &endptr, 10);
				if (*endptr != '\0' || ping->ttl <= 0 || ping->ttl > 255) {
					fprintf(stderr, "ping: invalid argument: '%s'\n", optarg);
					exit(1);
				}
				break;
			}

			case '?': print_help(); exit(0);
			default: fprintf(stderr, "Unknown option\n"); exit(1);
		}
	}

	if (optind >= argc || argv[optind][0] == '-') {
		fprintf(stderr, "ping: missing or invalid hostname\n");
		exit(2);
	}

	ping->hostname = argv[optind];

	if (optind + 1 < argc) {
		fprintf(stderr, "Too many arguments: ");
		for (int i = optind + 1; i < argc; i++)
			fprintf(stderr, "'%s' ", argv[i]);
		fprintf(stderr, "\n");
		exit(3);
	}
}
