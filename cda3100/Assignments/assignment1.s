    .data
    
prompt: .asciiz "Please enter three numbers."
    
    .text

main:
    la $a0,prompt
    li $v0,4
    syscall

    li $v0,5
    syscall
    move $t0,$v0

    li $v0,5
    syscall
    move $t1,$v0

    li $v0,5
    syscall
    move $t2,$v0

    add $t3,$t0,$t1
    add $t3,$t3,$t2

    move $a0,$t3
    li $v0,1
    syscall

    li $v0,10     #exit
    syscall