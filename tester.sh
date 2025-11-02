#!/bin/bash

FT_PING="./ft_ping"
SYS_PING=$(command -v ping)
TIMEOUT_DURATION=1

test_cases=(
	"8.8.8.8"
	"google.com"
	"localhost"
	"127.0.0.1"
	"192.0.2.123"
	"notarealhost.tld"
	"999.999.999.999"
	"invalid..address"
	"-v 8.8.8.8"
	"-v google.com"
	"-v 192.0.2.123"
	"-v notarealhost.tld"
	"-V"
	"-?"
	"--help"
	"-x 8.8.8.8"
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
	timeout -s SIGINT $TIMEOUT_DURATION $SYS_PING $args 2>&1
	local sys_exit=$?
	echo -e "${GRAY}[Exit code: $sys_exit]${NC}"

	echo -e "${BLUE}==============================${NC}"
	echo
}


for case in "${test_cases[@]}"; do
	run_test "$case"
	sleep 2
done