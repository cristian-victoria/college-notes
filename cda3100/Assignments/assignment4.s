    .data
    
A: .word 103, 33, 45, 87, 23, 89, 13, 15, 19, 77 #Initialize array A = [103, 33, 45, 87, 23, 89, 13, 15, 19, 77]
    
    .text

main:
    la $t0 A #$t0 has the base address of a0

    lw $s0, 12($t0)

    lw $s1, 20($t0)

    lw $s2, 28($t0)

    addi $t1, $s0, 13
    sub $t1, $t1, $s1

    addi $t2, $t2, 22

    mult $s2, $t2
    mflo $t2

    sub $t1, $t1, $t2

    sw $t1, 0($t0)

    move $a0, $t1
    li $v0, 1
    syscall

    li $v0, 10
    syscall