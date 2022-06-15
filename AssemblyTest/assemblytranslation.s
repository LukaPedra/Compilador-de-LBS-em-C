.text
.global function2

function1:
    push %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movl %edi, -28(%rbp)

    movl %edi, %r10d
    imull %edi, %r10d
    movl %r10d, %eax

    leave
    ret


function2:
    push %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movl %edi, -28(%rbp)


    movl %edi, %r11d
    cmpl $0, %r11d
    movl $0, %eax
    jne label
    leave
    ret

label:
    movl %edi, %r11d
    subl $1, %r11d
    movl %r11d, -4(%rbp)
    call function1
    movl %eax, -8(%rbp)
    movl -4(%rbp), %edi
    call function2
    movl -4(%rbp), %r10d
    addl -8(%rbp), %r10d
    movl %r10d, -4(%rbp)
    movl -4(%rbp), %eax
    leave
    ret
