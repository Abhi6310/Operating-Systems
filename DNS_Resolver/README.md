![Language: C](https://img.shields.io/badge/language-C-blue.svg)

A C program that reads hostnames from one or more input files, resolves each to an IPv4 address, and logs the results. Unresolved names are marked `NOT_RESOLVED`, and total runtime is printed on exit :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}.

## Table of Contents

- [Overview](#overview)  
- [Features](#features)  
- [Prerequisites](#prerequisites)  
- [Building](#building)  
- [Input Files](#input-files)  
- [Output Format](#output-format)  
- [Error Handling](#ErrorHandling)  
- [Runtime Reporting](#runtime-reporting)   

## Overview

`pa5_lookup` processes each data file line-by-line, uses `getaddrinfo()` to perform IPv4 DNS lookups, and writes `<hostname>, <IP-or-NOT_RESOLVED>` to the specified log file :contentReference[oaicite:2]{index=2}:contentReference[oaicite:3]{index=3}.

## Features

- Resolves hostnames to IPv4 only (`AF_INET`)  
- Processes up to 20 input files (`MaxInputFiles = 20`) :contentReference[oaicite:4]{index=4}:contentReference[oaicite:5]{index=5}  
- Skips blank lines  
- Marks failures as `NOT_RESOLVED`  
- Overwrites or creates the log file in write mode  
- Reports total runtime using `gettimeofday()` :contentReference[oaicite:6]{index=6}:contentReference[oaicite:7]{index=7}  

## Prerequisites

- GCC or any C99-compatible compiler  
- POSIX-compliant system (for `gettimeofday()`, `getaddrinfo()`, `inet_ntop()`)  

## Building

```bash
gcc -std=c99 -D_POSIX_C_SOURCE=200112L -Wall -Wextra -o pa5_lookup lookup.c
./pa5_lookup <log_file> <datafile1> [datafile2 … datafileN]

Each data file should list one domain name per line

The log file will then be formatted as <hostname>, <IPv4-address-or-NOT_RESOLVED>

For Example:
amazon.co.jp, 52.119.164.121
linkedin.com, 150.171.22.12
``` :contentReference[oaicite:10]{index=10}:contentReference[oaicite:11]{index=11}

## ErrorHandling

- **Missing arguments**: prints usage synopsis to stderr and exits with failure :contentReference[oaicite:12]{index=12}:contentReference[oaicite:13]{index=13}  
- **Too many input files**: prints `Too many input files (max 20)` to stderr and exits :contentReference[oaicite:14]{index=14}:contentReference[oaicite:15]{index=15}  
- **Unreadable data file**: prints `invalid file <filename>` to stderr, then continues :contentReference[oaicite:16]{index=16}:contentReference[oaicite:17]{index=17}  
- **I/O or system call errors**: prints descriptive messages to stderr; exits or continues based on recoverability  

## Runtime Reporting

Upon completion, the program prints to stdout:

./pa5_lookup: total time is X.XXXXXX seconds