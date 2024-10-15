# Introduction
This is the first ALTEQ implementation ported in Cortex-M4.
We used STM32429I-EVAL1 board, which has 128KB SRAM, 8MB SDRAM, and 180MHz frequency.

# What we did
- Replaced all of `mm_malloc` into static array, and deleted all of `mm_free`.
- Edited `STM32F429NIHX_FLASH.ld` to use SDRAM, which contains large data.
- Optimize modular arithmetics in ARM assembly(WIP).

# Benchmark
|Algorithm|    Keygen   |     Sign    |   Verify   | Optimize level |
|:-------:|:-----------:|:-----------:|:----------:|:--------------:|
|ALTEQ-fe1|  10777429   |  299911505  |  265298641 |      None      |