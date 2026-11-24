    .data

A: .word 2, 4, 8, 16, 32, 64, 65, 230 #Initialize array A = [2, 4, 8, 16, 32, 64, 65, 230]
trueMsg: .asciiz "true"
falseMsg: .asciiz "false"

    .text

main:
    la $t0, A           # Load address of the array into $t0
    li $t1, 7           # $t1 is the index counter for loop
    li $t2, 2           # $t2 will be used to double the items in the array

loop:
    lw $t3, 0($t0)      # Load the current array element into $t3
    lw $t4, 4($t0)      # Load the next array element into $t4
    mult $t3, $t2       # Multiply $t3 by 2
    mflo $t5            # Store the result into $t5

    bgt $t5, $t4, false # If $t5 > $t4, jump to false

    addiu $t5, $t4, 4   # Else, move to the next element in the array
    addi $t1, $t1, -1   # Decrement loop counter
    bgtz $t1, loop      # If counter > 0, loop again

    li $v0, 4           # Prepare the print syscall for "true"
    la $a0, trueMsg     # Output true message
    syscall 
    j end               # Jump to end

false:
    li $v0, 4           # Prepare the print syscall for "false"
    la $a0, falseMsg    # Output false message
    syscall

end:
    li $v0, 10          # End program
    syscall