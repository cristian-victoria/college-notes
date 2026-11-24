    .data
    
A: .word 99, 32, 40, 77, 23, 95, 11 #Initialize array A = [99, 32, 40, 77, 23, 95, 11]
    
    .text

main:
    la $t0 A #$t0 has the base address of a0

    lw $s0, 8($t0)

    lw $s1, 16($t0)

    add $t1, $s0, 9
    sub $t1, $t1, $s1
    sw $t1, 0($t0)

    move $a0, $t1
    li $v0, 1
    syscall

    li $v0, 10
    syscall