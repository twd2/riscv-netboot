.section .text.entrypoint, "ax", @progbits
.global _entrypoint
_entrypoint:
    j _start

.section .text.start, "ax", @progbits
.global _start
_start:
    la t0, trap_entry
    csrw mtvec, t0
    la sp, _end_of_stack - 0x10
    j start

.section .text.bootloader.start, "ax", @progbits
.global enter_bootloader
enter_bootloader:
    la sp, _end_of_stack - 0x10
    j bootloader

.section .text
.balign 8
trap_entry:
    # reuse main stack now
    la sp, _end_of_stack - 0x10
    call trap_handler
    j enter_bootloader

.global enter_kernel
enter_kernel:
    # a0: dtb
    # a1: kernel entry
    mv a2, a1
    mv a1, a0
    csrr a0, mhartid
    jr a2

.section .stack
_stack:
    .space 0x3f0
_end_of_stack:
