.data
    array: .word 5, 10, 15, 20, 25 
    searchElement: .word 15          
    foundMsg: .asciiz "Element found at index: "
    notFoundMsg: .asciiz "Element not found"
    newline: .asciiz "\n"

.text

.globl main

main:
    li $t1, 0                   
    lw $t2, searchElement       
    li $t3, -1                  

loop:
    lw $t0, array($t1)          
    beq $t0, $t2, found         
    addi $t1, $t1, 4            
    li $t4, 20                 
    blt $t1, $t4, loop          

    la $a0, notFoundMsg        
    li $v0, 4                  
    syscall
    j end                      
found:
    la $a0, foundMsg           
    li $v0, 4                  
    syscall

    li $v0, 1                   
    move $a0, $t1               
    srl $a0, $a0, 2             
    syscall

    la $a0, newline             
    li $v0, 4                   
    syscall

end:
    li $v0, 10                  
    syscall