# FT_PING

## Table of Contents
- [About](#about)
- [Installation](#installation)
- [Usage](#usage)
- [Key Concepts Learned](#key-concepts-learned)
- [Skills Developed](#skills-developed)
- [Project Overview](#project-overview)
- [Features Implemented](#features-implemented)
- [Program Architecture](#program-architecture)
- [Testing](#testing)
- [42 School Standards](#42-school-standards)


## About

This repository contains my implementation of the **ft_ping** project.
ft_ping is a recreation of the famous **ping** command-line utility, one of the most fundamental network diagnostic tools used worldwide since 1983.
Built entirely in **C**, this program sends ICMP (Internet Control Message Protocol) echo requests to network hosts and measures round-trip time, packet loss, and network reachability using **raw sockets** and low-level network programming.

The project demonstrates mastery of network protocols, raw socket programming, and system-level C programming while implementing the core functionality of the original ping utility.


## Installation

### Prerequisites
- **C Compiler** (gcc/clang)
- **Make** utility
- **UNIX/Linux environment** (raw socket programming)
- **Root/sudo privileges** (required for raw socket creation)

### Compilation
```bash
# Clone the repository
git clone https://github.com/TuroTheReal/ft_ping.git
cd ft_ping

# Compile the program
make

# Compile the program with bonus flag (i, c, w, t)
make bonus

# Clean object files
make clean

# Clean everything
make fclean

# Recompile
make re
```


## Usage

### Basic Usage
```bash
# Ping a host (requires sudo for raw sockets)
sudo ./ft_ping <hostname or IP>

# Example - Ping Google DNS
sudo ./ft_ping 8.8.8.8
sudo ./ft_ping google.com
```

### Command-Line Options
```bash
# Verbose mode - show detailed socket information
sudo ./ft_ping -v google.com

# Set number of packets to send
sudo ./ft_ping_bonus -c 5 8.8.8.8

# Set timeout in seconds
sudo ./ft_ping_bonus -w 10 google.com

# Set interval between packets (default: 1 second)
sudo ./ft_ping_bonus -i 0.5 8.8.8.8

# Set the time to live (default: 64)
sudo ./ft_ping_bonus -t 128

# Display help
./ft_ping -h/--help/-?
```

### Example Output
```
PING google.com (142.250.185.46) 56(84) bytes of data.
64 bytes from 142.250.185.46: icmp_seq=1 ttl=117 time=12.3 ms
64 bytes from 142.250.185.46: icmp_seq=2 ttl=117 time=11.8 ms
64 bytes from 142.250.185.46: icmp_seq=3 ttl=117 time=12.1 ms

--- google.com ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2004ms
rtt min/avg/max/mdev = 11.8/12.1/12.3/0.2 ms
```


## Key Concepts Learned

### Network Programming Fundamentals
- **Raw Socket Programming**: Creating and managing SOCK_RAW sockets for ICMP protocol
- **ICMP Protocol**: Understanding Internet Control Message Protocol for network diagnostics
- **Packet Structure**: Building and parsing IP headers, ICMP headers, and payload data
- **DNS Resolution**: Converting hostnames to IP addresses using getaddrinfo()

### Low-Level C Programming
- **Binary Data Manipulation**: Working with network byte order (htons/ntohs)
- **Checksum Calculation**: Implementing RFC-compliant ICMP checksum algorithms
- **Byte Alignment**: Handling structure padding and memory alignment for network packets
- **Pointer Arithmetic**: Navigating packet headers and extracting nested protocol information

### System Programming
- **Signal Handling**: Graceful interruption with SIGINT (Ctrl+C) for clean statistics display
- **Timer Management**: Precise timing with gettimeofday() and usleep() for intervals and timeouts
- **File Descriptor Multiplexing**: Using select() for non-blocking socket operations with timeouts
- **Privilege Management**: Understanding raw socket requirements and security implications

### Network Diagnostics
- **Round-Trip Time (RTT)**: Measuring network latency with microsecond precision
- **Packet Loss Detection**: Tracking transmitted vs. received packets for reliability metrics
- **TTL (Time To Live)**: Understanding hop counts and network path characteristics
- **ICMP Error Handling**: Processing Time Exceeded and Destination Unreachable messages


## Skills Developed

- **Network Protocol Implementation**: Professional-level understanding of ICMP and IP protocols
- **Raw Socket Mastery**: Low-level socket programming for custom packet crafting
- **System-Level C**: Advanced C programming for network and system applications
- **Precision Timing**: Microsecond-accurate time measurements and timeout handling
- **Statistical Analysis**: Computing min/avg/max/mdev for network performance metrics
- **Error Handling**: Robust handling of network errors, timeouts, and edge cases
- **Cross-Platform Compatibility**: Writing portable network code for UNIX-like systems


## Project Overview

ft_ping implements the core functionality of the ping utility, enabling network administrators and developers to diagnose network connectivity, measure latency, and detect packet loss.
The program sends ICMP Echo Request packets to a target host and listens for Echo Reply responses, calculating round-trip times and displaying comprehensive network statistics.

### Core Components

**ICMP Packet Builder**: Constructs valid ICMP Echo Request packets with proper headers, sequence numbers, checksums, and payload data following RFC 792 specifications.

**Raw Socket Manager**: Creates and configures raw sockets with appropriate privileges, handles socket options, and manages non-blocking I/O for efficient packet transmission and reception.

**DNS Resolver**: Converts human-readable hostnames to IP addresses using modern getaddrinfo() API, supporting both IPv4 addresses and domain name resolution.

**Packet Receiver**: Listens for ICMP responses using select() for timeout handling, parses IP and ICMP headers, validates packet authenticity, and extracts timing information.

**Statistics Calculator**: Tracks transmitted and received packets, computes packet loss percentage, calculates RTT statistics (min/avg/max), and computes standard deviation for latency measurements.

**Signal Handler**: Catches SIGINT interruptions for graceful shutdown, displays final statistics, and ensures proper cleanup of network resources.


## Features Implemented

### Core Ping Functionality
- **ICMP Echo Request/Reply**: Send and receive ICMP packets for network reachability testing
- **DNS Resolution**: Support for both hostnames and IP addresses
- **RTT Measurement**: Microsecond-precision round-trip time calculation
- **Packet Loss Detection**: Track and report lost packets

### Command-Line Options
- **-v (verbose)**: Display detailed socket and connection information
- **-c (count)**: Specify number of packets to send before stopping
- **-w (timeout)**: Set maximum execution time in seconds
- **-i (interval)**: Configure time between packet transmissions (default: 1s)
- **-h (help)**: Display usage information and available options
- **-t (ttl)**: Set the time to live of the packet before stopping

### Network Statistics
- **Packet Statistics**: Transmitted, received, and packet loss percentage
- **RTT Statistics**: Minimum, average, maximum, and standard deviation (mdev)
- **Total Time**: Duration of the entire ping session
- **TTL Display**: Show Time To Live values from received packets

### Error Handling
- **ICMP Time Exceeded**: Detect and report when TTL reaches zero
- **Destination Unreachable**: Handle various unreachable error codes
- **Request Timeouts**: Identify packets that don't receive responses
- **Packet Validation**: Filter out packets from other ping processes using PID matching

### Signal Management
- **SIGINT Handling**: Graceful shutdown on Ctrl+C with statistics display
- **Clean Termination**: Proper cleanup of sockets and resources
- **Statistics Preservation**: Display accurate stats even when interrupted


## Program Architecture

### ICMP Protocol Implementation
The program implements ICMP Echo Request (Type 8) and processes Echo Reply (Type 0) packets according to RFC 792 specifications, ensuring compatibility with standard network infrastructure.

### Checksum Algorithm
Uses the standard one's complement checksum algorithm for ICMP packet validation, ensuring data integrity during network transmission.

### Packet Identification
Each ping process uses its PID (Process ID) as the ICMP identifier to distinguish its packets from other concurrent ping instances on the same system.

### Timeout Mechanism
Implements deadline-based timeout handling using select() with dynamic timeout calculations, allowing the program to detect lost packets without blocking indefinitely.

### Statistics Calculation
Computes standard deviation using the computational formula for variance to measure RTT consistency and network stability.


## Testing

### Basic Connectivity Tests
```bash
# Test local loopback
sudo ./ft_ping 127.0.0.1

# Test external DNS servers
sudo ./ft_ping 8.8.8.8
sudo ./ft_ping 1.1.1.1

# Test with hostnames
sudo ./ft_ping google.com
sudo ./ft_ping github.com
```

### Option Testing
```bash
# Test verbose mode
sudo ./ft_ping -v google.com

# Test packet count
sudo ./ft_ping_bonus -c 10 8.8.8.8

# Test custom interval
sudo ./ft_ping_bonus -i 0.2 google.com

# Test timeout
sudo ./ft_ping_bonus -w 5 unreachable-host.com

# Combined options
sudo ./ft_ping_bonus -v -c 5 -i 0.5 8.8.8.8
```

### Edge Cases
```bash
# Test unreachable hosts
sudo ./ft_ping 192.168.255.255

# Test invalid hostnames
sudo ./ft_ping nonexistent.invalid

# Test with Ctrl+C interruption
sudo ./ft_ping google.com  # Press Ctrl+C after a few packets
```

### Comparison with Original Ping
```bash
# Compare output and statistics
ping -c 5 google.com
sudo ./ft_ping_bonus -c 5 google.com

# Verify RTT measurements
ping -c 5 8.8.8.8
sudo ./ft_ping_bonus -c 5 8.8.8.8
```


## 42 School Standards

### Project Standards
- ✅ No memory leaks (validated with valgrind)
- ✅ Raw socket implementation for ICMP protocol
- ✅ Proper DNS resolution and error handling
- ✅ RFC-compliant ICMP packet structure and checksum
- ✅ Accurate RTT statistics and packet loss calculation
- ✅ Norm compliance (42 coding standards)

### Network Requirements
- ✅ ICMP Echo Request/Reply implementation
- ✅ Support for both IPv4 addresses and hostnames
- ✅ Configurable packet count and intervals
- ✅ Timeout handling for lost packets
- ✅ Signal handling for graceful interruption

### Technical Requirements
- ✅ Microsecond-precision timing
- ✅ Standard deviation (mdev) calculation
- ✅ PID-based packet identification
- ✅ Non-blocking socket operations with select()
- ✅ Proper privilege handling for raw sockets

### Bonus Features (Optional)
- ✅ Timeout option (-w flag)
- ✅ Custom interval configuration (-i flag)
- ✅ Custom amout of packet sent (-c flag)
- ✅ Custom time to live for the packet (-t flag)


## Contact

- **GitHub**: [@TuroTheReal](https://github.com/TuroTheReal)
- **Email**: arthurbernard.dev@gmail.com
- **LinkedIn**: [Arthur Bernard](https://www.linkedin.com/in/arthurbernard92/)

---

<p align="center">
  <img src="https://img.shields.io/badge/Made%20with-C-blue.svg"/>
  <img src="https://img.shields.io/badge/Protocol-ICMP-green.svg"/>
  <img src="https://img.shields.io/badge/Networking-Raw%20Sockets-red.svg"/>
</p>
