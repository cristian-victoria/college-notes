 .data

A: .word 2, 4, 8, 16, 32, 64, 128, 256 #Initialize array A = [2, 4, 8, 16, 32, 64, 128, 256]
prompt: .asciiz "Please enter an integer.\n"

    .text

main:
    la $t0, A           # Load address of the array into $t0
    
    la $a0,prompt
    li $v0,4            # Display prompt to ask user for input
    syscall

    li $v0,5            # Take in integer input
    syscall
    move $t1,$v0        # $t1 = target

    li $t2, 0           # $t2 = lo
    li $t3, 7           # $t3 = hi = size - 1

while_loop:
    ble $t2, $t3, continue_search   # If lo <= high, continue search
    j not_found                     # Else, print -1

continue_search:
    add $t4, $t2, $t3   # $t4 = (lo + hi)
    move $t5, $0
    addi $t5, $t5, 2    # $t5 = constant 2
    div $t4, $t5        # $t4 = (lo + hi) / 2
    mflo $t4            # $t4 = mid
         
    mult $t4, $t5       
    mflo $t6           
    mult $t6, $t5
    mflo $t6            # $t6 = mid * 4
    
    add $t6, $t6, $t0   # $t6 now contains address of index mid in array
    lw $t7, 0($t6)      # $t7 = arr[mid]
    
    beq $t7, $t1, found # If arr[mid] == target, jump to found
    blt $t7, $t1, left  # Else, if arr[mid] < target, jump to left
    sub $t3, $t4, 1     # Else, hi = mid - 1

    j while_loop        # Jump to start of while loop

left:
    add $t2 $t4, 1      # lo = mid + 1
    j while_loop        # Jump to start of while loop

found:
    li $v0, 1           # Prepare the print syscall for int
    move $a0, $t4       # Output the mid if found
    syscall
    j end               # Jump to end

not_found:
    li $v0, 1           # Prepare the print syscall for int
    la $a0, -1          # Output -1 if not found
    syscall

end:
    li $v0, 10          # End program
    syscall