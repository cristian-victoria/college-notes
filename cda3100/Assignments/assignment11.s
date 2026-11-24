.data
    prompt: .asciiz "Input an integer: "
    comma: .asciiz ", "

.text
.globl main

main:
    li $v0, 4
    la $a0, prompt          # Print prompt
    syscall

    li $v0, 5
    syscall                 # Read integer n
    move $t0, $v0           # Move integer to $t0

    li $t1, 20
    ble $t0, $t1, initial   # Check if n is less than or equal to 20
    li $t0, 20              # If not, make $t0 equal to 20

initial:                    # Initialize Fibonacci numbers
    li $t2, 1               # $t2 = Fibonacci number n-2
    li $t3, 1               # $t3 = Fibonacci number n-1
    li $t4, 0               # $t4 = Counter

loop:
    move $a0, $t2
    li $v0, 1               # Print Fibonacci number
    syscall

    add $t5, $t2, $t3       # Calculate next Fibonacci number
    move $t2, $t3
    move $t3, $t5

    addi $t4, $t4, 1        # Increment counter

    blt $t4, $t0, commaLoop # Print comma and loop until counter == n

end:
    li $v0, 10              # End program
    syscall

commaLoop:
    li $v0, 4
    la $a0, comma           # Print comma
    syscall

    j loop