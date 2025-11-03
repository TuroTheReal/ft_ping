#!/bin/bash

FT_PING="./ft_ping_bonus"
SYS_PING="./ping_copy"
TIMEOUT_DURATION=5

test_cases=(
	"-c 2 8.8.8.8"
	"google.com -c 3"
	"-i 2 -c 3 8.8.8.8"
	"-i 0.2 -c 3 8.8.8.8"
	"-w 5 -c 3 8.8.8.8"
	"-w 0.5 -c 3 8.8.8.8"
	"-w 1 -c 2 192.0.2.1"
	"-t 1 -c 1 8.8.8.8"
	"-t 128 -c 2 8.8.8.8"
	"-c 3 -i 2 8.8.8.8"
	"-c 2 -w 2 8.8.8.8"
	"-c 2 -t 64 8.8.8.8"
	"-i 0.5 -w 2 -c 2 8.8.8.8"
	"-c 3 -i 1 -w 2 -t 64 8.8.8.8"
	"-c 3 -i 1 google.com"
	"-c 3 notarealhost.tld"
	"-c 0 8.8.8.8"
	"-c -3 8.8.8.8"
	"-i 0 -c 3 8.8.8.8"
	"-i -1 -c 3 8.8.8.8"
	"-w 0 -c 3 8.8.8.8"
	"-t 0 -c 2 8.8.8.8"
	"-t 64 -c 2 8.8.8.8"
	"-c abc 8.8.8.8"
	"-t abc 8.8.8.8"
)

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
GRAY='\033[0;90m'
NC='\033[0m'

run_test() {
	local args="$1"

	echo -e "${YELLOW}=== Testing: ${NC}$args ${YELLOW}===${NC}"

	echo -e "${GREEN}--- ft_ping ---${NC}"
	sudo timeout -s SIGINT $TIMEOUT_DURATION $FT_PING $args 2>&1
	local ft_exit=$?
	echo -e "${GRAY}[Exit code: $ft_exit]${NC}"

	echo -e "${GREEN}--- system ping ---${NC}"
	sudo timeout -s SIGINT $TIMEOUT_DURATION $SYS_PING $args 2>&1
	local sys_exit=$?
	echo -e "${GRAY}[Exit code: $sys_exit]${NC}"

	echo -e "${BLUE}==============================${NC}"
	echo
}

for case in "${test_cases[@]}"; do
	run_test "$case"
	sleep 2
done