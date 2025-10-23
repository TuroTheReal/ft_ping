#include "ft_ping.h"

void validate_options(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] == '-') {
			// Options longues valides
			if (strcmp(argv[i], "--help") != 0 &&
				strcmp(argv[i], "--ttl") != 0) {
				fprintf(stderr, "ping: unrecognized option '%s'\n", argv[i]);
				print_help();
				exit(1);
			}
		}
		else if (argv[i][0] == '-' && argv[i][1] != '\0') {
			// Vérifier que c'est une option courte valide (1 seul caractère après -)
			if (argv[i][2] != '\0' && argv[i][1] != '-') {
				// C'est une chaîne comme -help, -ttl, etc.
				fprintf(stderr, "ping: unrecognized option '%s'\n", argv[i]);
				print_help();
				exit(1);
			}
		}
	}
}

void parse_args(int argc, char** argv, t_ping *ping) {
	int opt;
	static struct option long_options[] = {
		{"ttl", required_argument, NULL, 't'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};

	validate_options(argc, argv);

	opterr = 0;  // Désactiver msg erreur auto getopt

	while ((opt = getopt_long(argc, argv, "vV?c:i:W:t:h", long_options, NULL)) != -1) {
		switch (opt) {
			case 'v': ping->verbose = 1; break;
			case 'V': print_version(); exit(0);
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
				if (ping->interval < 0.2 && getuid() != 0) {
					fprintf(stderr, "ping: cannot flood; minimal interval allowed is 200ms\n");
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
			case '?':
				if (optopt == 0) {
					// -?
					print_help();
					exit(0);
				} else {
					// Option invalide
					fprintf(stderr, "ping: invalid option -- '%c'\n", optopt);
					print_help();
					exit(1);
				}
				break;
			case 'h':
				print_help();
				exit(0);
			default:
				fprintf(stderr, "Unknown option\n");
				exit(1);
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