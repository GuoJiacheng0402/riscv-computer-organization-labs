.section .text
.global sum_to_n

sum_to_n:
    li t0, 0
    li t1, 0

loop:
    bgt t1, a0, end
    add t0, t0, t1
    addi t1, t1, 1
    j loop

end:
    mv a0, t0
    ret
