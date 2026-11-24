    .data
        firstNum: .float 2.33
        secondNum: .float 4.55
        equalMsg: .asciiz "Equal\n"
        notEqualMsg: .asciiz "Not equal\n"

    .text

main:
    lwc1 $f0, firstNum      # $f0 = 2.33
    lwc1 $f1, secondNum     # $f1 = 4.55

    c.eq.s $f0, $f1         # if $f0 == $f1, jump to notEqual
    bc1t equal

notEqual:
    li $v0, 4               # Prepare the print syscall for "Not equal"
    la $a0, notEqualMsg     # Output not equal message
    syscall

    add.s $f2, $f0, $f1     # $f2 = 2.33 + 4.55
    j end

equal:
    li $v0, 4               # Prepare the print syscall for "equal"
    la $a0, equalMsg        # Output equal message
    syscall
    
    sub.s $f2, $f1, $f0     # $f2 = 4.55 - 2.33

end:
    li $v0, 2               # Prepare the print syscall for float
    mov.s $f12, $f2         # Output the floating point value
    syscall

    li $v0, 10              # End program
    syscall