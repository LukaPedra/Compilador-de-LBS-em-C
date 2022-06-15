.text
.global function1

function1:
pushq %rbp
movq %rsp, %rbp
subq $32, %rsp
movl %edi, -28(%rbp)

movl %edi, %r10d

cmpl $0, %r10d
movl $1, %eax
jne label
leave
ret

label:
movl %edi, %r10d
addl $0, %r10d
movl %r10d, -4(%rbp)

movl -4(%rbp), %r10d
subl $1, %r10d
movl %r10d, -8(%rbp)

movl -8(%rbp), %edi

call function1
movl %eax, -8(%rbp)

movl -4(%rbp), %r10d
imull -8(%rbp), %r10d
movl %r10d, -4(%rbp)

movl -4(%rbp), %eax

leave
ret
