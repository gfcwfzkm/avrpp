# Notes and Logs

This document will serve both as my notes and my session log. I won't be able to work on it every evening, so taking clear notes what I was focused on the last session will be important to quickly get back into the project.

Whatever interesting quirks, conclusions or things I learn and find out, be it AVR or C++ related, will also be dumped here.

## 11.11.2026

The AVR family started in a weird, limited way - with a 3-level deep hardware stack on the AT90S1200! It seems it's not even really supported by AVR-GCC, so this 'origin' AVR core will probably not be implemented. It doesn't make sense to test and simulate such a limited architecture. A proper Stack Pointer was introduced later, which starts at the end of SRAM and grows downwards.

The core usually has 32 8-bit work-registers (familiar if you're used to ARM or RISC-V!), but with some quirks where the last 6 registers can be used as 3 16-bit registers for indexing. Stack pointer and program counter are not integrated in the work registers, called X, Y and Z registers. There is a special STATUS REG (`SREG`) which contains the `Global Interrupt Enable` flag among other ALU-related flags.

There are three different busses. The I/O Memory bus, Program Memory bus and the Data bus. Being a Harvard Architecture, the seperate Program and Data bus makes sense but the I/O Memory bus is interesting. It only has a 6-bit address space, with the `SREG` register being at the last address `0x3F`. The rest often contains the peripherals, Stack Pointer and I/O Ports on it. In addition, the 32 work registers and the I/O Memory bus is accessible from the Data bus, where the lowest 32 locations access the work registers (0x00 - 0x1F), the next 32 bytes access the I/O Memory bus (0x20 - 0x5F), before SRAM is finally accessed.

The I/O Memory bus can perform direct bit manipulations on the destinations, which isn't possible on the data bus. So I can only assume this weird bus access has been done, such that indexed memory access could read the work registers and peripherals as well?

Regarding the different families, the AVR ISA Manual lists 6 different versions, but if you include the instruction and core variations in between, I come to 14 different variants!

Fundamentally they are quite similar. Usually they differ either by some extra instructions to access larger memory ranges, have or lack EEPROMs, or for the tiniest AVR controllers the lack of the lower 16 work registers.