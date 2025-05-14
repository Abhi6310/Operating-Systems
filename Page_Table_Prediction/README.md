# Paging Simulator

[![Language: C](https://img.shields.io/badge/language-C-blue.svg)](https://www.gnu.org/software/gcc/)

A virtual‐memory environment simulator that models process scheduling, page faults, and custom page‐replacement strategies, plus tools for logging and interactive visualization.

## Table of Contents

- [Overview](#overview)  
- [Features](#features)  
- [Getting Started](#getting-started)  
  - [Prerequisites](#prerequisites)  
  - [Installation](#installation)  
- [Usage](#usage)  
  - [Basic Simulation](#basic-simulation)  
  - [Common Options](#common-options)  
- [Page‐Replacement Strategy](#page-replacement-strategy)  
- [Data Analysis & Visualization](#data-analysis--visualization)  
- [Project Structure](#project-structure)  
- [Contributing](#contributing)  
- [License](#license)  

---

## Overview

This simulator models multiple processes running on a (configurable) number of CPUs in a virtual‐memory system. It tracks page references, faults, and replacements, and can emit CSV logs for post‐run analysis and graphing.

## Features

- **Process scheduling** with load, block, branch, and deadlock events  
- **Customizable page‐replacement** (Markov‐predictor–based in `pager-predict.c`)  
- **Fine‐grained logging** of page faults, branches, loads, etc.  
- **CSV output** for history and page events (`output.csv`, `pages.csv`)  
- **Interactive visualization** via R script (`see.R`)  

## Getting Started

### Prerequisites

- GCC (or any C99‐compatible compiler)  
- R (version ≥ 3.5)  

### Installation

```bash
git clone https://github.com/Abhi6310/Operating-Systems/tree/main/Page_Table_Prediction
cd paging-simulator
make
