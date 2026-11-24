    .data
        prompt: .asciiz "Please enter a positive integer.\n"
        result: .space 100
    .text

main:
    la $a0,prompt
    li $v0,4                # Print prompt
    syscall

    li $v0,5
    syscall         
    move $t1,$v0            # $t1 holds input

    move $t2, $0
    addi $t2, $t2, 2        # $t2 = constant 2

convert:
    div $t1, $t2            # $t1 = $t1 / 2
    mflo $t1                # Quotent in $t1
    mfhi $t4                # Remainder in $t4

    sb $t5, buffer

    bne $t1, $t0, convert   # if $t1 != 0, repeat

end:
    li $v0, 4               # Prepare the print syscall for str
    la $a0, buffer          # Output $t1
    syscall
    
    li $v0, 10              # End program
    syscall