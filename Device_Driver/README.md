# Simple Character Device Driver

[![Language: C](https://img.shields.io/badge/language-C-blue.svg)](https://www.gnu.org/software/gcc/) [![License: MIT](https://img.shields.io/badge/license-MIT-yellow.svg)](LICENSE)

A Linux kernel module that implements a basic character device (`simple_character_device`) with an in-kernel buffer, open/close counters, and support for `read`, `write`, and `llseek` operations :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}.

## Table of Contents

- [Overview](#overview)  
- [Features](#features)  
- [Prerequisites](#prerequisites)  
- [Building](#building)  
- [Loading & Unloading](#loading--unloading)  
- [Creating Device Node](#creating-device-node)  
- [Usage](#usage)  
- [Testing](#testing)  
- [Project Structure](#project-structure)  
- [Contributing](#contributing)  
- [License](#license)  

---

## Overview

This module registers a character device with major number **511** and maintains a **900-byte** kernel buffer. It logs how many times the device has been opened or closed, and enforces bounds checking on all read/write/seek operations. :contentReference[oaicite:2]{index=2}:contentReference[oaicite:3]{index=3}

## Features

- **Major number**: 511  
- **Buffer size**: 900 bytes  
- **Operations**: `open`, `release`, `read`, `write`, `llseek`  
- **Open/close counters** logged via `printk`  
- **Boundary checks** with clamping and proper error codes  
- **Graceful error handling** (`-ENODEV`, `-EINVAL`, `-EFAULT`, `-ENOSPC`) :contentReference[oaicite:4]{index=4}:contentReference[oaicite:5]{index=5}

## Prerequisites

- Linux kernel headers (`linux-headers-$(uname -r)`)  
- GCC, make, and kernel build tools  

## Building

Create a **Makefile** in the same directory:

```makefile
obj-m += pa6_char_driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
run:
    make