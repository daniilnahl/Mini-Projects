# Custom Linux System Call — Raspberry Pi 3B+

Implemented and tested a custom Linux system call on a Raspberry Pi 3B+ running a modified ARM64 Linux kernel.

## Overview

This project demonstrates modifying the Linux kernel to introduce a custom system call that can be invoked by a user-space C application.

## Implementation

- Cross-compiled a custom ARM64 Linux kernel on an Ubuntu host.
- Registered a custom system call and implemented its handler.
- Used `printk()` to generate messages in the kernel log.
- Installed and booted the modified kernel on a Raspberry Pi 3B+.
- Developed a user-space C application to invoke the custom syscall.
- Verified functionality using `dmesg`.

## Challenges

Debugged kernel deployment issues involving missing kernel modules, including the failure of ZRAM initialization due to a mismatch between the installed kernel and available modules.

## Technologies

**C | Linux Kernel | ARM64 | Raspberry Pi | GCC | Cross-Compilation | Embedded Linux**

## Repository Contents

- `kernel-changes.patch` — Source modifications implementing the system call.
- `test_syscall.c` — User-space program used to invoke the syscall.
- `evidence/dmesg_output.txt` — Kernel log demonstrating execution.

## Learning Outcomes

Gained practical experience navigating Linux kernel source code, understanding system call registration, cross-compiling kernels, deploying kernel modules, and debugging embedded Linux systems.
