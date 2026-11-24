 .data
    
prompt1: .asciiz "Please enter an integer.\n"
prompt2: .asciiz "Please enter another integer.\n"
promptNoOutput: .asciiz "No output.\n"
    
    .text

main:
    la $a0,prompt1
    li $v0,4
    syscall

    li $v0,5
    syscall
    move $t0,$v0

    la $a0,prompt2
    li $v0,4
    syscall

    li $v0,5
    syscall
    move $t1,$v0

    beq $t0,$t1,equal
    blt $t0,$t1,lessThan

    li $v0,4
    la $a0,promptNoOutput
    syscall
    j end

equal:
    add $t2,$t1,$t0
    move $a0,$t2
    li $v0,1
    syscall
    j end

lessThan:
    mult $t0,$t1
    mflo $t2
    move $a0,$t2
    li $v0,1
    syscall
    j end

end:
    li $v0, 10
    syscall