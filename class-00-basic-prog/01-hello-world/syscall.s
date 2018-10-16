//#include <sysdep.h>
/* Please consult the file sysdeps/unix/sysv/linux/x86-64/sysdep.h for
   more information about the value -4095 used below.  */
/* Usage: long syscall (syscall_number, arg1, arg2, arg3, arg4, arg5, arg6)
   We need to do some arg shifting, the syscall_number will be in
   rax.  */
    .text
    .globl syscall
    .type syscall @function
    
    //ENTRY (syscall)
syscall:    
        movq %rdi, %rax                /* Syscall number -> rax.  */
        movq %rsi, %rdi                /* shift arg1 - arg5.  */
        movq %rdx, %rsi
        movq %rcx, %rdx
        movq %r8, %r10
        movq %r9, %r8
        movq 8(%rsp),%r9        /* arg6 is on the stack.  */
        syscall                        /* Do the system call.  */
//        cmpq $-4095, %rax        /* Check %rax for error.  */
//        jae SYSCALL_ERROR_LABEL        /* Jump to error handler if error.  */
        ret                        /* Return to caller.  */
//PSEUDO_END (syscall)
