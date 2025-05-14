# Thread-Safe Circular Buffer

[![Language: C](https://img.shields.io/badge/language-C-blue.svg)](https://www.gnu.org/software/gcc/)

A bounded, thread-safe circular buffer implementation in C using POSIX threads, with a multi-producer/multi-consumer test harness.

## Table of Contents

- [Overview](#overview)  
- [Features](#features)  
- [Prerequisites](#prerequisites)  
- [Building](#building)  
- [Running the Test](#running-the-test)  
- [API Reference](#api-reference)  
- [Project Structure](#project-structure)  
- [Contributing](#contributing)  
- [License](#license)  

---

## Overview

This library provides a fixed-size circular buffer (`array`) that blocks producers when full and consumers when empty, using `pthread_mutex` and `pthread_cond` for synchronization :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}.

The `test_array.c` program demonstrates four producer threads generating hostnames and four consumer threads retrieving them, terminating via a “poison pill” message :contentReference[oaicite:2]{index=2}:contentReference[oaicite:3]{index=3}.

## Features

- Fixed-capacity circular buffer (`ARRAY_SIZE = 8` by default)  
- Thread safety with mutex and condition variables  
- Blocking `put`/`get` semantics  
- Clean teardown of synchronization primitives  

## Prerequisites

- GCC (or any C99-capable compiler)  
- POSIX threads (`-pthread`)  

## Building

```bash
git clone git clone https://github.com/Abhi6310/Operating-Systems/tree/main/Thread_Safe_Array
cd circular-buffer
gcc -std=c99 -Wall -pthread array.c test_array.c -o test_array

## Running the Test
./test_array
