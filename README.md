# Operating-Systems
This repository contains various operating systems projects and implementations focusing on core OS concepts like thread safety, file operations, and network services.

## Projects

### Thread Safe Array

A thread-safe array implementation that allows concurrent access from multiple threads without data corruption or race conditions.

**Features:**
- Thread-safe operations (add, remove, access)
- Mutex-based synchronization
- Performance optimized for concurrent access

### Page Table Prediction

A virtual-memory environment simulator that models process scheduling, page faults, and implements custom page-replacement strategies.

**Features:**
- Process scheduling with load, block, branch, and deadlock events
- Customizable page-replacement strategies
- Fine-grained logging of page faults, branches, loads
- CSV output for history and page events analysis
- Interactive visualization capabilities

### File Tester

A utility for testing file operations and performance in different operating system environments.

**Features:**
- File creation, reading, and writing benchmarks
- Permission testing
- Cross-platform compatibility

### Device Driver

A Linux kernel module that implements a basic character device with an in-kernel buffer and support for standard file operations.

**Features:**
- Major number 511 with 900-byte kernel buffer
- Operations: open, release, read, write, llseek
- Open/close counters logged via printk
- Boundary checks with proper error handling
- Graceful error handling for various failure conditions

### DNS Resolver

Implementation of a DNS resolver that translates domain names to IP addresses.

**Features:**
- Support for A, AAAA, MX, and CNAME records
- Caching for improved performance
- Configurable timeout and retry mechanisms

## Getting Started

### Prerequisites

- C/C++ compiler (GCC or Clang recommended)
- Make or CMake for building
- POSIX-compliant operating system (Linux/Unix) for full functionality

### Installation

```bash
git clone https://github.com/Abhi6310/Operating-Systems.git
cd Operating-Systems
make
```

## Usage

Each project contains its own directory with specific usage instructions. See individual project READMEs for detailed information.

