# RISC-V Computer Organization Labs


> [!CAUTION]
> **Academic Integrity Notice (SCUT)**
> <br>This repository contains my personal lab work for SCUT *Computer Organization*. The author is currently **actively enrolled in this course**. Do not copy, reuse, or submit any part of this repo (including modified versions) for graded coursework. Violations may be treated as plagiarism under SCUT rules. See [LICENSE](./LICENSE) for restrictions.


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
