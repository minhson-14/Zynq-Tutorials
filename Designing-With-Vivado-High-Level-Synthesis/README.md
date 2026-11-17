# Designing with Vivado/Vitis High-Level Synthesis

![Vitis HLS](https://img.shields.io/badge/Vitis_HLS-2022.2-blue)
![C++](https://img.shields.io/badge/Language-C%2B%2B-green)
![Target](https://img.shields.io/badge/Target-Zynq--7020-lightgrey)
![Status](https://img.shields.io/badge/Status-Completed-success)

This repository contains a High-Level Synthesis lab that implements and optimizes a 5x5 matrix multiplication accelerator for a Zynq-7000 FPGA target.

The project demonstrates how C++ code can be simulated, synthesized, optimized through HLS directives, and prepared for hardware implementation.

## Project Goals

- Build a C++ matrix multiplication kernel for HLS.
- Verify algorithm correctness with a C testbench.
- Explore synthesis reports for latency, initiation interval, and resource usage.
- Apply optimization directives such as `PIPELINE` and `ARRAY_RESHAPE`.
- Study how Vitis HLS maps C function arguments to RTL interfaces.

## Design Under Test

The top-level function computes:

```text
C = A x B
```

for two 5x5 matrices.

```cpp
void matrix_mult(
    mat_a_t    inA[N][N],
    mat_b_t    inB[N][N],
    mat_prod_t prod[N][N]
);
```

Data types:

| Signal | Type | Description |
|---|---|---|
| `inA[5][5]` | `ap_int<8>` | Input matrix A |
| `inB[5][5]` | `ap_int<8>` | Input matrix B |
| `prod[5][5]` | `ap_int<16>` | Output matrix C |

## Lab Structure

| Exercise | Focus | Main Files |
|---|---|---|
| 3A | Project creation and C simulation | `tut3A/` |
| 3B | Optimization and solution comparison | `tut3B/` |
| 3C | Interface synthesis exploration | `tut3C/` |

## Repository Layout

```text
.
|-- tut3A/
|   |-- matrix_mult.h
|   |-- matrix_mult.cpp
|   |-- matrix_mult_test.cpp
|   `-- run_hls.tcl
|-- tut3B/
|   |-- matrix_mult.h
|   |-- matrix_mult.cpp
|   `-- matrix_mult_test.cpp
|-- tut3C/
|   |-- matrix_mult.h
|   |-- matrix_mult.cpp
|   |-- matrix_mult_test.cpp
|   `-- run_hls.tcl
`-- README.md
```

Generated Vitis HLS project directories are excluded from version control. The repository keeps only the source files and scripts required to reproduce the work.

## Optimization Summary

The lab compares multiple HLS solutions. The most important results observed during synthesis are summarized below.

### Timing

| Solution | Target Clock | Estimated Clock | Result |
|---|---:|---:|---|
| Solution 1 | 10 ns | 6.967 ns | Met |
| Solution 2 | 10 ns | 6.706 ns | Met |
| Solution 3 | 5 ns | 3.430 ns | Met |
| Solution 4 | 5 ns | 3.570 ns | Met |
| Solution 5 | 5 ns | 3.372 ns | Met |

### Latency and Interval

| Solution | Main Directive | Latency | Interval | Relative Result |
|---|---|---:|---:|---|
| 1 | Baseline | 81 cycles | 82 cycles | Reference |
| 2 | Pipeline inner product loop | 131 cycles | 132 cycles | Slower due to flattened loop trip count |
| 3 | Pipeline column loop | 86 cycles | 87 cycles | Faster wall time with 5 ns clock |
| 4 | Pipeline + array reshape | 63 cycles | 64 cycles | Improved memory parallelism |
| 5 | Function-level pipeline | 27 cycles | 13 cycles | Best latency in this experiment |

## What This Demonstrates

- How loop structure affects HLS scheduling.
- Why an inner-loop pipeline can improve II but still increase total latency.
- How memory port limitations can prevent the target II from being achieved.
- How `ARRAY_RESHAPE` can expose additional parallel memory access.
- How aggressive function-level pipelining/unrolling trades resource usage for latency.

## Running the Lab

### Exercise 3A

```bash
cd tut3A
vitis_hls -f run_hls.tcl
```

This creates the HLS project, sets `matrix_mult` as the top function, adds the source and testbench, and runs C simulation.

### Exercise 3B

Open Vitis HLS 2022.2 and create multiple solutions from the `tut3B` source files. Apply directives through the GUI:

```text
PIPELINE
ARRAY_RESHAPE
```

Then compare synthesis reports across solutions.

### Exercise 3C

```bash
cd tut3C
vitis_hls -f run_hls.tcl
```

Use the synthesis report to inspect generated RTL interfaces and control protocols.

## Toolchain

| Tool | Version |
|---|---|
| Vitis HLS | 2022.2 |
| Language | C++ with Xilinx arbitrary precision types |
| Target device | `xc7z020clg484-1` |

## Portfolio Notes

This project is useful as a compact example of FPGA design space exploration with HLS. It shows not only a working accelerator kernel, but also the reasoning process behind directive selection and performance/resource tradeoffs.
