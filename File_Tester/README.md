# File Tester

[![Language: C](https://img.shields.io/badge/language-C-blue.svg)](https://www.gnu.org/software/gcc/)

A simple interactive file manipulation tool written in C. Allows you to read, write, seek within, and test error handling on a target file via a command-line menu :contentReference[oaicite:0]{index=0}:contentReference[oaicite:1]{index=1}.

## Table of Contents

- [Overview](#overview)  
- [Features](#features)  
- [Prerequisites](#prerequisites)  
- [Building](#building)  
- [Usage](#usage)  

---

## Overview

This program opens a specified file in read/write mode and presents an interactive menu to:
- Read a user-specified number of bytes  
- Write arbitrary data to the file  
- Seek to a given offset with selectable `whence` (`SEEK_SET`, `SEEK_CUR`, `SEEK_END`)  
- Exit cleanly  
Error conditions (invalid filename, I/O errors, invalid inputs) are handled with descriptive messages :contentReference[oaicite:2]{index=2}:contentReference[oaicite:3]{index=3}.

## Features

- **Read**: Prompt for byte count, read from current offset, display contents.  
- **Write**: Prompt for string input, write to current offset.  
- **Seek**: Prompt for offset and whence value (0, 1, 2), move file pointer.  
- **Exit**: Clean shutdown on ‘e’ or EOF (Ctrl+D).  
- Robust error checking for invalid inputs and system call failures.  

## Prerequisites

- GCC or any C99-capable compiler  
- POSIX-compatible environment (for `open`, `read`, `write`, `lseek`)  

## Building

```bash
git clone https://github.com/yourusername/file-tester.git
cd file-tester
gcc -std=c99 -Wall -Wextra -o pa5test pa5test.c

## Usage

./pa5test <filename>
Startup shows you this: Option (r for read, w for write, s for seek, e for exit):
Enter:
r → then number of bytes to read
w → then the data string to write
s → then offset value, then whence (0, 1, 2)
e → exit program