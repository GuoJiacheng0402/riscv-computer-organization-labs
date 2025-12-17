# RISC-V Computer Organization Labs

> [!WARNING]
> **Academic Integrity Notice (SCUT)**
>
> This repository contains **personal laboratory work** for the course **Computer Organization and Architecture** at SCUT.
>
> ⚠️ **This repository is publicly available for educational reference only.**
>
> - The experiments in this repository are **individually completed** by the author.
> - **Directly copying, reusing, or submitting any part of this repository as course assignment work
>   constitutes plagiarism**, which is a **serious violation of SCUT academic integrity regulations**.
> - Students taking the same or similar courses **must complete all laboratory work independently**.
>
> 🚫 **Do NOT submit this code (or any modified version of it) as your own coursework.**
>
> The author is currently **actively enrolled in this course**, and this repository is made public
> **solely as the source-code reference attached to the author's own assignment submission**.
> The author **does not authorize** any form of academic reuse for graded coursework.

---



## Platform & Tools

- **CPU Core**: [HBirdv2 E203](https://github.com/riscv-mcu/e203_hbirdv2)
- **FPGA Board**: Nuclei DDR200T (Xilinx XC7A200T)
- **IDE**: Nuclei Studio
---

## Laboratory Overview

### [Lab 1: RISC-V ISA Basics & Assembly](./Lab1_ISA_and_Assembly)
- Understanding RISC-V register conventions (x0-x31).
- Analyzing C-to-Assembly mapping using **LST (Listing) views**.
- Implementing mixed C/Assembly programming (`sum_to_n` subroutine).
- Mastering function calling conventions (`a0`-`a7`, `ra`, `sp`).

### [Lab 2: Memory-Mapped I/O (MMIO)](./Lab2_MMIO_GPIO_UART)
- Concepts of Address Space and **Memory-Mapped I/O**.
- Controlling GPIO (LEDs/Switches) and UART registers.
- Low-level register access using `volatile` pointers and macro abstractions.

### [Lab 3: Interrupts vs. Polling](./Lab3_Interrupts_vs_Polling)
- Comparing **Polling** (busy-wait) vs. **Interrupt-driven** designs.
- Configuring the **PLIC** (Platform-Level Interrupt Controller).
- Handling RISC-V **M-mode Traps**, `mcause` analysis, and Context Saving.

### [Lab 4: Pipeline Optimization & Benchmarking](./Lab4_Pipeline_Optimization)
- Porting and running the **CoreMark** benchmark.
- Performance analysis using hardware counters (`mcycle`, `minstret`).
- Calculating **CPI** (Cycles Per Instruction) and **IPC**.
- Identifying pipeline hazards and implementing micro-architectural optimizations (e.g., Data Forwarding).

---


## Usage Policy

✔ Allowed:
- Reading the code for **learning and understanding**
- Referencing architecture-level ideas and implementation patterns

✘ Not Allowed:
- Copying code for **course assignments**
- Submitting this repository (or parts of it) as your own work
- Using this code for any graded academic submission

---

## Disclaimer

This repository is provided **as-is**, without any guarantee of correctness or completeness.
The author assumes **no responsibility** for any academic misconduct resulting from
unauthorized use of this repository.
